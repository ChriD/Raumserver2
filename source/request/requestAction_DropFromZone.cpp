
#include <raumserver/request/requestAction_DropFromZone.h>

namespace Raumserver
{
    namespace Request
    {
        RequestAction_DropFromZone::RequestAction_DropFromZone(std::string _url) : RequestAction(_url)
        {
            action = RequestActionType::RAA_DROPFROMZONE;
        }


        RequestAction_DropFromZone::RequestAction_DropFromZone(std::string _path, std::string _query) : RequestAction(_path, _query)
        {
            action = RequestActionType::RAA_DROPFROMZONE;
        }


        RequestAction_DropFromZone::~RequestAction_DropFromZone()
        {
        }


        bool RequestAction_DropFromZone::isValid()
        {
            bool isValid = RequestAction::isValid();

            // examples for valid requests:
            // raumserver/controller/dropFromZone?id=Schlafzimmer
            // raumserver/controller/dropFromZone?id=uuid:3f68f253-df2a-4474-8640-fd45dd9ebf88

            auto id = getOptionValue("id");
            if (id.empty())
            {
                logError("'id' option is needed to execute 'dropFromZone' command!", CURRENT_FUNCTION);
                isValid = false;
            }

            return isValid;
        }

       
        bool RequestAction_DropFromZone::executeAction()
        {
            std::uint16_t processTime = 0;
            auto id = getOptionValue("id");
            
            if (!id.empty())
            {
                std::string roomUDN;
                bool roomOk = false;

                getManagerEngineer()->getDeviceManager()->lock();
                getManagerEngineer()->getZoneManager()->lock();

                try
                {
                    roomUDN = getRoomUDNFromId(id);
                    roomOk = getManagerEngineer()->getZoneManager()->existsRoomUDN(roomUDN);
                }
                catch (...)
                {
                    logError("Unknown Exception!", CURRENT_POSITION);
                }

                getManagerEngineer()->getDeviceManager()->unlock();
                getManagerEngineer()->getZoneManager()->unlock();

                if (roomOk)
                {
                    getManagerEngineer()->getZoneManager()->dropRoom(roomUDN);
                    if (sync)
                    {
                        bool zoneOfroomEmpty = false;
                        // wait until room is dropped from zone or a timout happens      
                        // INFO: We may register a signal of the zoneManager like "zoneOfRoomChanged" and poll a var which will change on this signal
                        while (!zoneOfroomEmpty && processTime <= timeout)
                        {
                            getManagerEngineer()->getDeviceManager()->lock();
                            getManagerEngineer()->getZoneManager()->lock();

                            try
                            {
                                zoneOfroomEmpty = getManagerEngineer()->getZoneManager()->isRoomInZone(roomUDN, "");
                            }
                            catch (...)
                            {
                                logError("Unknown Exception!", CURRENT_POSITION);
                            }

                            getManagerEngineer()->getDeviceManager()->unlock();
                            getManagerEngineer()->getZoneManager()->unlock();


                            std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeForRequestActionKernelResponse));
                            processTime += waitTimeForRequestActionKernelResponse;
                        }

                        if (processTime > timeout)
                            logWarning("Timout on request (" + std::to_string(timeout) + "): " + getRequestInfo(), CURRENT_FUNCTION);
                    }
                }
                else
                {
                    logError("Room with UDN '" + roomUDN  +"' not found!", CURRENT_FUNCTION);
                }
            }                

            return true;
        }      
    }
}
