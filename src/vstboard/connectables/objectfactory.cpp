/******************************************************************************
#    Copyright 2010 Rapha�l Fran�ois
#    Contact : ctrlbrk76@gmail.com
#
#    This file is part of VstBoard.
#
#    VstBoard is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VstBoard is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "objectfactory.h"
#include "audiodevice.h"
#include "audiodevicein.h"
#include "audiodeviceout.h"
#include "mididevice.h"
#include "midisender.h"
#include "miditoautomation.h"
#include "hostcontroller.h"
#include "container.h"
#include "bridge.h"
#include "mainhost.h"
//#include "loader.h"

#ifdef VSTSDK
    #include "vstplugin.h"
    #include "../vst/cvsthost.h"
#endif

using namespace Connectables;

ObjectFactory *ObjectFactory::theObjFactory=0;
QMutex ObjectFactory::singletonMutex;

ObjectFactory * ObjectFactory::Get()
{
    singletonMutex.lock();
    if(!theObjFactory)
        theObjFactory = new ObjectFactory();
    singletonMutex.unlock();

    return theObjFactory;
}

ObjectFactory::ObjectFactory() :
    cptListObjects(50)
{
}

ObjectFactory::~ObjectFactory()
{
    Clear();
    theObjFactory=0;
}

void ObjectFactory::Clear()
{
    cptListObjects=0;

//    hashObjects::iterator i = listObjects.begin();
//    while(!listObjects.isEmpty()) {
//        i.value()->Close();
//        i = listObjects.erase(i);
//    }

    listObjects.clear();
}

QSharedPointer<Object> ObjectFactory::GetObjectFromId(int id)
{
    if(!listObjects.contains(id)) {
        debug("ObjectFactory::GetObjectFromId : object not found %d",id)
        return QSharedPointer<Object>();
    }

    return listObjects.value(id);
}

void ObjectFactory::ResetSavedId()
{
    hashObjects::iterator i = listObjects.begin();
    while(i != listObjects.end()) {
        QSharedPointer<Object> objPtr = i.value().toStrongRef();
        if(objPtr.isNull()) {
            i=listObjects.erase(i);
        } else {
            //don't reset forcced ids
            if(i.key()>=50) {
                objPtr->ResetSavedIndex();
            }
            ++i;
        }
    }
}

int ObjectFactory::IdFromSavedId(int savedId)
{
    hashObjects::const_iterator i = listObjects.constBegin();
    while(i != listObjects.constEnd()) {
        QSharedPointer<Object> objPtr = i.value().toStrongRef();
        if(objPtr && objPtr->GetSavedIndex()==savedId) {
            return i.key();
        }
        ++i;
    }
    debug("object factory idfromsavedid: id not found %d",savedId)
    return -1;
}

Pin *ObjectFactory::GetPin(const QModelIndex & index)
{
    ConnectionInfo info = index.data(UserRoles::connectionInfo).value<ConnectionInfo>();
    return GetPin(info);
}

Pin *ObjectFactory::GetPin(const ConnectionInfo &pinInfo)
{
    if(!listObjects.contains(pinInfo.objId)) {
        debug("objectfactory getpin : obj not found")
        return 0;
    }

    QSharedPointer<Object> objPtr = listObjects.value(pinInfo.objId).toStrongRef();
    if(objPtr)
        return objPtr->GetPin(pinInfo);

    return 0;
}

void ObjectFactory::RemoveObject(int id)
{
    if(!listObjects.contains(id)) {
        debug(QString("ObjectFactory::RemoveObject object %1 already deleted").arg(id).toAscii())
        return;
    }

     QSharedPointer<Object> objPtr = listObjects.take(id);
     if(objPtr.isNull())
         return;
//     objPtr->Hide();
}

QSharedPointer<Object> ObjectFactory::GetObj(const QModelIndex & index)
{
    //the object is not created, do it
    if(!index.data(UserRoles::value).isValid()) {
        return NewObject( index.data(UserRoles::objInfo).value<ObjectInfo>() );
    }

    //or return the existing object
    return GetObjectFromId(index.data(UserRoles::value).toInt());
}

//QSharedPointer<Object> ObjectFactory::NewObject(ObjType::Enum type, int id, QString name, int forceObjId)
//{
//    ObjectInfo info;
//    info.objType = type;
//    info.id = id;
//    info.name = name;
//    info.forcedObjId = forceObjId;

//    return NewObject(info);
//}

QSharedPointer<Object> ObjectFactory::NewObject(const ObjectInfo &info)
{
    int objId = cptListObjects;
    if(info.forcedObjId) {
        objId = info.forcedObjId;
    }

    Object *obj=0;

    switch(info.nodeType) {

        case NodeType::container :
            switch(info.objType) {
                case ObjType::Container:
                    obj = new Container(objId, info);
                    break;

                case ObjType::MainContainer:
                    obj = new MainContainer(objId, info);
                    break;

                default :
                    debug("ObjectFactory::NewObject : unknown object type")
                    return QSharedPointer<Object>();
            }
            break;

        case NodeType::bridge :
            obj = new Bridge(objId, info);
            break;

        case NodeType::object :

            switch(info.objType) {

        //        case ObjType::AudioInterface:
        //            //create both input and output
        //            if(info.inputs>0) {
        //                info.objType = ObjType::AudioInterfaceIn;
        //                NewObject(info);
        //            }
        //            if(info.outputs>0) {
        //                info.objType = ObjType::AudioInterfaceOut;
        //                NewObject(info);
        //            }
        //            break;

                case ObjType::AudioInterfaceIn:
                    obj = new AudioDeviceIn(objId, info);
                    break;

                case ObjType::AudioInterfaceOut:
                    obj = new AudioDeviceOut(objId, info);
                    break;

                case ObjType::MidiInterface:
                    obj = new MidiDevice(objId, info);
                    break;

                case ObjType::MidiSender:
                    obj = new MidiSender(objId);
                    break;

                case ObjType::MidiToAutomation:
                    obj = new MidiToAutomation(objId);
                    break;

                case ObjType::HostController:
                    obj = new HostController(objId);
                    break;

        #ifdef VSTSDK
                case ObjType::VstPlugin:
                    obj = new VstPlugin(objId, info);
                    break;
        #endif

                case ObjType::dummy :
                    obj = new Object(objId, info);
                    break;

                default:
                    debug("ObjectFactory::NewObject : unknown object type")
                    return QSharedPointer<Object>();
            }
            break;


        default :
            debug("ObjectFactory::NewObject unknown nodeType")
            return QSharedPointer<Object>();
    }



    QSharedPointer<Object> sharedObj(obj);
    listObjects.insert(objId,sharedObj.toWeakRef());

    if(!obj->Open()) {
        listObjects.remove(objId);
        sharedObj.clear();
        return QSharedPointer<Object>();
    }

    if(info.forcedObjId) {
        obj->ResetSavedIndex(info.forcedObjId);
    } else {
        cptListObjects++;
    }

    return sharedObj;

}


//QDataStream & ObjectFactory::toStream(QDataStream & out) const
//{
//    out << (qint16)listObjects.size();
//    hashObjects::const_iterator i = listObjects.constBegin();
//    foreach(QSharedPointer<Object> objPtr, listObjects) {
//        if(!objPtr.isNull()) {
//            out<<(quint8)objPtr->GetType();
//            out<<(quint16)objPtr->GetIdentity();
//            out<<objPtr->GetIdentityString();
//            out<<*objPtr.data();

//            debug(QString("save %1 %2").arg(objPtr->GetIndex()).arg(objPtr->objectName()).toAscii())
//        }
//    }
//    return out;
//}

//QDataStream & ObjectFactory::fromStream(QDataStream & in)
//{
//    ResetSavedId();

//    quint16 nbObj;
//    in >> nbObj;
//    for(quint16 i=0; i<nbObj; i++) {
//        quint8 objType;
//        in>>objType;
//        quint16 identity;
//        in>>identity;
//        QString identityString;
//        in>>identityString;

//        debug(QString("load %1 %2").arg(objType).arg(identityString).toAscii())

//       QSharedPointer<Object> objPtr = NewObject((ObjType::Enum)objType,identity,identityString);
//       if(!objPtr.isNull())
//           in>>*objPtr.data();
//    }
//    return in;
//}

//QDataStream & operator<< (QDataStream & out, const Connectables::ObjectFactory& value)
//{
//    return value.toStream(out);
//}

//QDataStream & operator>> (QDataStream & in, Connectables::ObjectFactory& value)
//{
//    return value.fromStream(in);
//}
