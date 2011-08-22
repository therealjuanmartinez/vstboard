/**************************************************************************
#    Copyright 2010-2011 Rapha�l Fran�ois
#    Contact : ctrlbrk76@gmail.com
#
#    This file is part of VstBoard.
#
#    VstBoard is free software: you can redistribute it and/or modify
#    it under the terms of the under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VstBoard is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    under the terms of the GNU Lesser General Public License for more details.
#
#    You should have received a copy of the under the terms of the GNU Lesser General Public License
#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include "object.h"
#include "globals.h"
#include "mainhost.h"
#include "renderer/pathsolver.h"
#include "container.h"
#include "events.h"
#include "meta/metapinslist.h"
#include "meta/metaobjviewattrib.h"

using namespace Connectables;

/*!
    \class Connectables::Object
    \brief virtual object, children of containers, parents of pins
*/

/*!
  Constructor, used by ObjectFactory
  \param host parent MainHost
  \param index unique id
  \param info ObjectInfo defining this object
  */
Object::Object(MainHost *myHost, MetaData &info) :
    QObject(),
    MetaObjEngine(info,myHost),
    parked(false),
    listenProgramChanges(true),
    myHost(myHost),
    savedIndex(0),
    sleep(true),
    currentProgram(0),
    currentProgId(TEMP_PROGRAM),
    closed(true)
{

    SET_MUTEX_NAME(objMutex,"objMutex "+GetMetaData<QString>(metaT::ObjName));
    SetType(MetaType::object);

    connect(this, SIGNAL(UndoStackPush(QUndoCommand*)),
            myHost, SLOT(UndoStackPush(QUndoCommand*)));

    setObjectName(QString("%1.%2").arg( GetMetaData<QString>(metaT::ObjName) ).arg( ObjId() ));
//    SetName(QString("%1.%2").arg( objInfo.objName ).arg( ObjectInfo::ObjId() ));
    doublePrecision=myHost->doublePrecision;

    {
        MetaPinsList l(0,myHost);
        l.SetName("listAudioPinIn");
        l.SetMedia(MediaTypes::Audio);
        l.SetDirection(Directions::Input);
        l.SetParent(this);
        listAudioPinIn = new PinsList(l,this);
        pinLists << listAudioPinIn;
    }

    {
        MetaPinsList l(0,myHost);
        l.SetName("listAudioPinOut");
        l.SetMedia(MediaTypes::Audio);
        l.SetDirection(Directions::Output);
        l.SetParent(this);
        listAudioPinOut = new PinsList(l,this);
        pinLists << listAudioPinOut;
    }

    {
        MetaPinsList l(0,myHost);
        l.SetName("listMidiPinIn");
        l.SetMedia(MediaTypes::Midi);
        l.SetDirection(Directions::Input);
        l.SetParent(this);
        listMidiPinIn = new PinsList(l,this);
        pinLists << listMidiPinIn;
    }

    {
        MetaPinsList l(0,myHost);
        l.SetName("listMidiPinIn");
        l.SetMedia(MediaTypes::Midi);
        l.SetDirection(Directions::Output);
        l.SetParent(this);
        listMidiPinOut = new PinsList(l,this);
        pinLists << listMidiPinOut;
    }

    {
        MetaPinsList l(0,myHost);
        l.SetName("listParameterPinIn");
        l.SetMedia(MediaTypes::Parameter);
        l.SetDirection(Directions::Input);
        l.SetParent(this);
        listParameterPinIn = new PinsList(l,this);
        pinLists << listParameterPinIn;
    }

    {
        MetaPinsList l(0,myHost);
        l.SetName("listParameterPinOut");
        l.SetMedia(MediaTypes::Parameter);
        l.SetDirection(Directions::Output);
        l.SetParent(this);
        listParameterPinOut = new PinsList(l,this);
        pinLists << listParameterPinOut;
    }

#ifdef SCRIPTENGINE
    QScriptValue scriptObj = myHost->scriptEngine->newQObject(this);
    myHost->scriptEngine->globalObject().setProperty(QString("Obj%1").arg(ObjId()), scriptObj);
#endif
}

/*!
  Destructor
  */
Object::~Object()
{
    pinLists.clear();

    if(ContainerId()) {
        QSharedPointer<Container>cntPtr = myHost->objFactory->GetObjectFromId( ContainerId() ).staticCast<Container>();
        if(cntPtr) {
            cntPtr->OnChildDeleted(this);
        }
    }
    Close();

    myHost->objFactory->RemoveObject(ObjId());
}

/*!
  Called by ObjectFactory after the object creation
  \return true on success, false if the object should be deleted
  */
bool Object::Open()
{
    closed=false;
    return true;
}

/*!
  Cleanup before destruction
  \return false if the object is already closed
  */
bool Object::Close()
{
    if(closed)
        return false;

    SetSleep(true);
    Hide();
    closed=true;

    foreach(ObjectProgram *prg, listPrograms) {
        delete prg;
    }
    listPrograms.clear();

    return true;
}

/*!
  Hide the object and all its pins
  \todo not used anymore except for bridges
  */
void Object::Hide() {
    foreach(PinsList *lst, pinLists) {
        lst->Hide();
    }
}

/*!
  Toggle sleep mode, the object is not rendered when sleeping
  */
void Object::SetSleep(bool sleeping)
{
    objMutex.lock();
    sleep = sleeping;
    objMutex.unlock();
}

/*!
  Retrive current sleep state
  */
bool Object::GetSleep()
{
    QMutexLocker l(&objMutex);
    return sleep;
}

/*!
  Check if the objects program has been modified
  \return true if modified
  */
bool Object::IsDirty()
{
    if(!currentProgram)
        return false;
    return currentProgram->IsDirty();
}

/*!
  Set the current program dirty flag
  Called by ParameterPin
  */
void Object::OnProgramDirty()
{
    if(!currentProgram)
        return;

    currentProgram->SetDirty();
}

/*!
  Unload current progam
  */
void Object::UnloadProgram()
{
    currentProgram=0;
    currentProgId=EMPTY_PROGRAM;
}

/*!
  Save the current program
  */
void Object::SaveProgram()
{
    if(!currentProgram || !currentProgram->IsDirty())
        return;

    currentProgram->Save(listParameterPinIn,listParameterPinOut);
}

/*!
  Load a program
    a new program is created if needed
    drop the current program if one is loaded
    \param prog program number
  */
void Object::LoadProgram(int prog)
{
    //if prog is already loaded, update model
    if(prog==currentProgId && currentProgram) {
        return;
    }

    //if a program is loaded, unload it without saving
    int progWas = currentProgId;
    if(currentProgId!=EMPTY_PROGRAM && currentProgram)
        UnloadProgram();

    currentProgId=prog;

    if(!listPrograms.contains(currentProgId))
        listPrograms.insert(currentProgId,new ObjectProgram(listParameterPinIn,listParameterPinOut));

    currentProgram=listPrograms.value(currentProgId);
    currentProgram->Load(listParameterPinIn,listParameterPinOut);


    //if the loaded program was a temporary prog, delete it
    if(progWas==TEMP_PROGRAM) {
        delete listPrograms.take(TEMP_PROGRAM);
    }
}

/*!
  Remove a program from the program list
  */
void Object::RemoveProgram(int prg)
{
    if(prg == currentProgId) {
        LOG("removing current program ! "<<prg<<objectName());
        return;
    }

    if(!listPrograms.contains(prg)) {
        LOG("prog not found"<<prg);
        return;
    }
    delete listPrograms.take(prg);
}

/*!
  Prepare for a new rendering
  Called one time at the beginning of the loop
  */
void Object::NewRenderLoop()
{
    if(sleep)
        return;

    foreach(Pin *pin, listAudioPinIn->listPins) {
        pin->NewRenderLoop();
    }
}

/*!
  Get the pin corresponding to the info
  \param pinInfo a ConnectionInfo describing the pin
  \return a pointer to the pin, 0 if not found
  */
Pin * Object::GetPin(const MetaData &pinInfo)
{
    Pin* pin=0;
    bool autoCreate=false;

    if(GetMetaData<int>(metaT::ObjType) == ObjTypes::Dummy || GetMetaPtr<QString*>(metaT::errorMessage)!=0)
        autoCreate=true;

    foreach(PinsList *lst, pinLists) {
        if(lst->GetMetaData<MediaTypes::Enum>(metaT::Media) == pinInfo.GetMetaData<MediaTypes::Enum>(metaT::Media)
                && lst->GetMetaData<Directions::Enum>(metaT::Direction) == pinInfo.GetMetaData<Directions::Enum>(metaT::Direction)) {
            pin=lst->GetPin(pinInfo.GetMetaData<int>(metaT::PinNumber),autoCreate);
            if(pin)
                return pin;
        }
    }
//    LOG("pin not found"<<pinInfo.toString());
    return 0;
}

/*!
  Called when a parameter pin has changed
  \param pinInfo the modified pin
  \param value the new value
  */
void Object::OnParameterChanged(const MetaData &pinInfo, float value)
{
    //editor pin
    if(pinInfo.GetMetaData<int>(metaT::PinNumber)==FixedPinNumber::editorVisible) {
        int val = static_cast<ParameterPin*>(listParameterPinIn->listPins.value(FixedPinNumber::editorVisible))->GetIndex();
        if(val)
            QTimer::singleShot(0, this, SLOT(OnShowEditor()));
        else
            QTimer::singleShot(0, this, SLOT(OnHideEditor()));
    }
}

/*!
  Toggle the editor (if the object has one) by changing the editor pin value
  \param visible true to show, false to hide
  */
void Object::ToggleEditor(bool visible)
{
    ParameterPin *pin = static_cast<ParameterPin*>(listParameterPinIn->listPins.value(FixedPinNumber::editorVisible));
    if(!pin)
        return;
    pin->ChangeValue(visible);
}

/*!
  Get the current learning state
  \return LearningMode::Enum can be learn, unlearn or off
  */
LearningMode::Enum Object::GetLearningMode()
{
    if(!listParameterPinIn->listPins.contains(FixedPinNumber::learningMode))
        return LearningMode::off;

    return (LearningMode::Enum)static_cast<ParameterPin*>(listParameterPinIn->listPins.value(FixedPinNumber::learningMode))->GetIndex();
}

/*!
  Set the object view status (position, size, ...) defined by the container
  \param[in] attr an ObjectContainerAttribs
  */
void Object::SetContainerAttribs(const MetaObjViewAttrib &attr)
{
    SetMeta(metaT::Position, attr.Position());
    SetMeta(metaT::EditorVisible, attr.EditorVisible());
    SetMeta(metaT::EditorSize, attr.EditorSize());
    SetMeta(metaT::EditorPosition, attr.EditorPosition());
    SetMeta(metaT::EditorVScroll, attr.EditorVScroll());
    SetMeta(metaT::EditorHScroll, attr.EditorHScroll());
    UpdateView();
}

/*!
  Get the object view status, the status is saved by the container in a ContainerProgram
  \param[out] attr an ObjectContainerAttribs containing the object status
  */
void Object::GetContainerAttribs(MetaObjViewAttrib &attr)
{
    attr.SetPosition( GetMetaData<QPointF>(metaT::Position));
    attr.SetEditorVisible( GetMetaData<bool>(metaT::EditorVisible));
    attr.SetEditorSize( GetMetaData<QSize>(metaT::EditorSize));
    attr.SetEditorPosition( GetMetaData<QPoint>(metaT::EditorPosition));
    attr.SetEditorVScroll( GetMetaData<int>(metaT::EditorVScroll));
    attr.SetEditorHScroll( GetMetaData<int>(metaT::EditorHScroll));
}

/*!
  Copy the position, editor visibility and learning state, used by SceneModel when a plugin is replaced
  \param objPtr destination object
  */
void Object::CopyStatusTo(QSharedPointer<Object>objPtr)
{
    MetaObjViewAttrib attr;
    GetContainerAttribs(attr);
    objPtr->SetContainerAttribs(attr);

    Connectables::ParameterPin* oldEditPin = static_cast<Connectables::ParameterPin*>(listParameterPinIn->GetPin(FixedPinNumber::editorVisible));
    Connectables::ParameterPin* newEditPin = static_cast<Connectables::ParameterPin*>(objPtr->listParameterPinIn->GetPin(FixedPinNumber::editorVisible));
    if(newEditPin && oldEditPin)
        newEditPin->ChangeValue(oldEditPin->GetValue());

    Connectables::ParameterPin* oldLearnPin = static_cast<Connectables::ParameterPin*>(listParameterPinIn->GetPin(FixedPinNumber::learningMode));
    Connectables::ParameterPin* newLearnPin = static_cast<Connectables::ParameterPin*>(objPtr->listParameterPinIn->GetPin(FixedPinNumber::learningMode));
    if(newLearnPin && oldLearnPin)
        newLearnPin->ChangeValue(oldLearnPin->GetValue());
}

void Object::SetBufferSize(unsigned long size)
{
    foreach(Pin *pin, listAudioPinIn->listPins) {
        static_cast<AudioPin*>(pin)->SetBufferSize(size);
    }
    foreach(Pin *pin, listAudioPinOut->listPins) {
        static_cast<AudioPin*>(pin)->SetBufferSize(size);
    }
}

void Object::UserRemovePin(const MetaData &info)
{
    if(info.GetMetaData<MediaTypes::Enum>(metaT::Media)!=MediaTypes::Parameter)
        return;

    if(!info.GetMetaData<bool>(metaT::Removable))
        return;

    switch(info.GetMetaData<int>(metaT::Direction)) {
        case Directions::Input :
            listParameterPinIn->AsyncRemovePin( info.GetMetaData<int>(metaT::PinNumber) );
            OnProgramDirty();
            break;
        case Directions::Output :
            listParameterPinOut->AsyncRemovePin( info.GetMetaData<int>(metaT::PinNumber) );
            OnProgramDirty();
            break;
    }
}

void Object::UserAddPin(const MetaData &info)
{
    if(info.GetMetaData<MediaTypes::Enum>(metaT::Media)!=MediaTypes::Parameter)
        return;

    switch(info.GetMetaData<int>(metaT::Direction) ) {
        case Directions::Input :
            listParameterPinIn->AsyncAddPin( info.GetMetaData<int>(metaT::PinNumber) );
            OnProgramDirty();
            break;
        case Directions::Output :
            listParameterPinOut->AsyncAddPin( info.GetMetaData<int>(metaT::PinNumber) );
            OnProgramDirty();
            break;
    }
}

PinsList* Object::GetPinList(Directions::Enum dir, MediaTypes::Enum type) const {
    foreach(PinsList *lst, pinLists) {
        if(lst->GetMetaData<Directions::Enum>(metaT::Direction) == dir
            && lst->GetMetaData<MediaTypes::Enum>(metaT::Media) == type)
            return lst;
    }

    return 0;
}

/*!
  Called by PinsList to create a pin
  \param info ConnectionInfo defining the pin to be created
  \return a pointer to the new pin, 0 if not created
  */
Pin* Object::CreatePin(MetaPin &info)
{
    switch(info.GetMetaData<int>(metaT::Direction)) {
        case Directions::Input :
            switch(info.GetMetaData<int>(metaT::Media)) {
                case MediaTypes::Audio : {
                    info.SetMeta(metaT::ObjName, QString("AudioIn%1").arg(info.GetMetaData<int>(metaT::PinNumber)));
                    return new AudioPin(this,info,myHost->GetBufferSize(),doublePrecision);
                }

                case MediaTypes::Midi : {
                    info.SetMeta(metaT::ObjName, QString("MidiIn%1").arg(info.GetMetaData<int>(metaT::PinNumber)));
                    return new MidiPinIn(this,info);
                }
                default :
                    return 0;
            }
            break;

        case Directions::Output :
            switch(info.GetMetaData<int>(metaT::Media)) {
                case MediaTypes::Audio : {
                    info.SetMeta(metaT::ObjName, QString("AudioOut%1").arg(GetMetaData<int>(metaT::PinNumber)));
                    return new AudioPin(this,info,myHost->GetBufferSize(),doublePrecision);
                }

                case MediaTypes::Midi : {
                    info.SetMeta(metaT::ObjName, QString("MidiOut%1").arg(GetMetaData<int>(metaT::PinNumber)));
                    return new MidiPinOut(this,info);
                }
                default :
                    return 0;
            }
            break;

        default :
            return 0;
    }

    return 0;
}

/*!
  Put the object in a stream
  \param[in] out a QDataStream
  \return the stream
  */
QDataStream & Object::toStream(QDataStream & out) const
{
    out << (qint16)ObjId();
    out << sleep;
    out << listenProgramChanges;

    out << (quint16)listPrograms.size();
    hashPrograms::const_iterator i = listPrograms.constBegin();
    while(i!=listPrograms.constEnd()) {
        out << (quint16)i.key();
        out << *i.value();
        ++i;
    }

    out << (quint16)currentProgId;
    return out;
}

/*!
  Load the object from a stream
  \param[in] in a QDataStream
  \return the stream
  */
bool Object::fromStream(QDataStream & in)
{
//    LoadProgram(TEMP_PROGRAM);

    quint32 id;
    in >> id;
    savedIndex=id;
    in >> sleep;
    in >> listenProgramChanges;

    quint16 nbProg;
    in >> nbProg;
    for(quint16 i=0; i<nbProg; i++) {
        quint16 progId;
        in >> progId;

        ObjectProgram *prog = new ObjectProgram();
        in >> *prog;
        if(listPrograms.contains(progId))
            delete listPrograms.take(progId);
        listPrograms.insert(progId,prog);
    }

    quint16 savedProgId;
    in >> savedProgId;

    if(in.status()!=QDataStream::Ok) {
        LOG("err"<<in.status());
        return false;
    }

    return true;
}

void Object::ProgramToStream (int progId, QDataStream &out)
{
    bool dirty = IsDirty();
    ObjectProgram *prog = 0;

    if(progId == currentProgId) {
        prog = new ObjectProgram(*currentProgram);
        prog->Save(listParameterPinIn,listParameterPinOut);
    } else {
        prog = listPrograms.value(progId,0);
    }

    if(!prog) {
        out << (quint8)0;
        return;
    }
    out << (quint8)1;

    out << (quint8)dirty;

    out << *prog;

    if(progId == currentProgId)
        delete prog;
}

void Object::ProgramFromStream (int progId, QDataStream &in)
{
    quint8 valid=0;
    in >> valid;
    if(valid!=1)
        return;

    quint8 dirty;
    in >> dirty;

    if(progId == currentProgId) {
        UnloadProgram();
        currentProgram = new ObjectProgram();
        in >> *currentProgram;
        if(dirty)
            currentProgram->SetDirty();
    } else {
        ObjectProgram *prog = new ObjectProgram();
        in >> *prog;

        if(listPrograms.contains(progId))
            delete listPrograms.take(progId);
        listPrograms.insert(progId,prog);
    }
}

/*!
  overload stream out
  */
QDataStream & operator<< (QDataStream & out, const Connectables::Object& value)
{
    return value.toStream(out);
}

/*!
  overload stream in
  */
//QDataStream & operator>> (QDataStream & in, Connectables::Object& value)
//{
//    return value.fromStream(in);
//}