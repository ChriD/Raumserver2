
#include <raumserver/request/requestAction.h>
#include <raumserver/request/requestActions.h>

namespace Raumserver
{
    namespace Request
    {
        RequestAction::RequestAction(std::string _url) : RaumserverBaseMgr()
        {
            url = _url;
            query = "";
            sync = true;
            error = "";
            waitTimeAfterExecution = 0;
            timeout = 5000;
            waitTimeForRequestActionKernelResponse = 25;
            action = RequestActionType::RAA_UNDEFINED;
        }

        RequestAction::RequestAction(std::string _path, std::string _query) : RaumserverBaseMgr()
        {
            url = _path;
            query = _query;
            sync = true;
            error = "";
            waitTimeAfterExecution = 0;
            timeout = 5000;
            waitTimeForRequestActionKernelResponse = 25;
            action = RequestActionType::RAA_UNDEFINED;
        }


        RequestAction::~RequestAction()
        {
        }


        bool RequestAction::isStackable()
        {
            return true;
        }


        bool RequestAction::isAsyncExecutionAllowed()
        {
            return true;
        }

        RequestActionType RequestAction::getActionType()
        {
            return action;
        }


        void RequestAction::parseQueryOptions()
        {
            if (query.empty())
                query = Raumkernel::Tools::UriUtil::getQueryFromUrl(url);
            requestOptions = Raumkernel::Tools::UriUtil::parseQueryString(query);
        }


        bool RequestAction::isValid()
        {       
            // try to parse the query options to the options map for further use
            parseQueryOptions();

            if (!isAsyncExecutionAllowed() && !sync)
                return false;
            return true;
        }


        std::string RequestAction::getOptionValue(std::string _key, std::string _default)
        {
            _key = Raumkernel::Tools::StringUtil::tolower(_key);
            auto it = requestOptions.find(_key);
            if (it == requestOptions.end())
                return _default;            
            return it->second;
        }


        std::vector<std::string> RequestAction::getOptionValueMultiple(std::string _key, std::string _delimiter)
        {
            std::vector<std::string> values;
            auto optionValue = getOptionValue(_key, "");
            if (!optionValue.empty())
                values = Raumkernel::Tools::StringUtil::explodeString(optionValue, _delimiter);
            return values;
        }


        std::string RequestAction::getRoomUDNFromId(std::string _id)
        {
            auto roomUDN = getManagerEngineer()->getZoneManager()->getRoomUDNForRoomName(_id);
            if (roomUDN.empty())
                roomUDN = _id;
            // check if room UDN is valid, otherwise return empty string
            if (!getManagerEngineer()->getZoneManager()->existsRoomUDN(roomUDN))
            {
                logError("Room for ID '" + _id  + "' not found", CURRENT_FUNCTION);
                return "";
            }
            return roomUDN;
        }       


        std::string RequestAction::getZoneUDNFromId(std::string _id)
        {
            std::string zoneUDN = "", roomUDN = "";
            roomUDN = getRoomUDNFromId(_id);
            if (!roomUDN.empty())
                zoneUDN = getManagerEngineer()->getZoneManager()->getZoneUDNForRoomUDN(roomUDN);
            if (zoneUDN.empty())
                zoneUDN = _id;
            // check if zone UDN is valid, otherwise return an empty string
            if (!getManagerEngineer()->getZoneManager()->existsZoneUDN(zoneUDN))
            {
                logError("Zone for ID '" + _id + "' not found", CURRENT_FUNCTION);
                return "";
            }
            return zoneUDN;
        }


       Raumkernel::Devices::MediaRenderer* RequestAction::getMediaRenderer(std::string _id)
        {
            Raumkernel::Devices::MediaRenderer* renderer = nullptr;
            std::string rendererUDN;

            if (!_id.empty())
            {
                // TODO: remove locks and do in reuqest itself???
                //getManagerEngineer()->getDeviceManager()->lock);
                //getManagerEngineer()->getZoneManager()->lockLists();

                try
                {
                    auto roomUDN = getRoomUDNFromId(_id);
                    if (roomUDN.empty())
                        rendererUDN = _id;
                    else
                        rendererUDN = getManagerEngineer()->getZoneManager()->getRendererUDNForRoomUDN(roomUDN);                
                    if (!rendererUDN.empty())
                        renderer = getManagerEngineer()->getDeviceManager()->getMediaRenderer(rendererUDN);
                }
                catch (...)
                {
                    logError("Unresolved Error!", CURRENT_FUNCTION);
                }

                //getManagerEngineer()->getZoneManager()->unlockLists();
                //getManagerEngineer()->getDeviceManager()->unlock();
            }
            return renderer;
        }
          

        Raumkernel::Devices::MediaRenderer_RaumfeldVirtual* RequestAction::getVirtualMediaRenderer(std::string _id)
        {
            Raumkernel::Devices::MediaRenderer_RaumfeldVirtual* virtualRenderer = nullptr;

            if (!_id.empty())
            {
                //getManagerEngineer()->getDeviceManager()->lock();
                //getManagerEngineer()->getZoneManager()->lockLists();

                try
                {

                    std::string zoneUDN = "", rendererUDN = "";
                    zoneUDN = getZoneUDNFromId(_id);
                    rendererUDN = getManagerEngineer()->getZoneManager()->getRendererUDNForZoneUDN(zoneUDN);
                    if (!rendererUDN.empty())
                        virtualRenderer = dynamic_cast<Raumkernel::Devices::MediaRenderer_RaumfeldVirtual*>(getManagerEngineer()->getDeviceManager()->getMediaRenderer(rendererUDN));
                }
                catch (...)
                {
                    logError("Unresolved Error!", CURRENT_FUNCTION);
                }

                //getManagerEngineer()->getZoneManager()->unlockLists();
                //getManagerEngineer()->getDeviceManager()->unlock();
            }
            return virtualRenderer;
        }


        Raumkernel::Devices::MediaRenderer_RaumfeldVirtual* RequestAction::getVirtualMediaRendererFromUDN(std::string _udn)
        {
            Raumkernel::Devices::MediaRenderer_RaumfeldVirtual* virtualRenderer = nullptr;

            if (!_udn.empty())
            {                
                //getManagerEngineer()->getDeviceManager()->lock();
                //getManagerEngineer()->getZoneManager()->lockLists();

                try
                {                   
                    virtualRenderer = dynamic_cast<Raumkernel::Devices::MediaRenderer_RaumfeldVirtual*>(getManagerEngineer()->getDeviceManager()->getMediaRenderer(_udn));
                }
                catch (...)
                {
                    logError("Unresolved Error!", CURRENT_FUNCTION);
                }

                //getManagerEngineer()->getZoneManager()->unlockLists();
                //getManagerEngineer()->getDeviceManager()->unlock();                
            }
            return virtualRenderer;
        }


        void RequestAction::logError(const std::string &_log, const std::string &_location)
        {
            RaumserverBaseMgr::logError(_log, _location);
            if (!error.empty()) error += "\n";
            error += _log;
        }


        void RequestAction::logCritical(const std::string &_log, const std::string &_location)
        {
            RaumserverBaseMgr::logCritical(_log, _location);
            if (!error.empty()) error += "\n";
            error += _log;
        }


        bool RequestAction::execute()
        {            
            bool ret = false;
            std::uint32_t waitTime = waitTimeAfterExecution;

            auto syncString = getOptionValue("sync");
            if (syncString == "false" || syncString == "0")
                sync = false;

            // check if the request is valid (mostly used for checking mandatory request options)
            if (isValid())
            {
                try
                {

                    // TODO: Check if system is online, otherwise don't execute!                    

                    auto measurePoint1 = std::chrono::system_clock::now().time_since_epoch();

                    ret = executeAction();

                    auto measurePoint2 = std::chrono::system_clock::now().time_since_epoch();

                    // put out request process time information
                    auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(measurePoint2).count() - std::chrono::duration_cast<std::chrono::milliseconds>(measurePoint1).count();
                    logDebug("Request duration: " + std::to_string(durationMS) + "ms: " + getRequestInfo(), CURRENT_FUNCTION);

                    // after execution of the request there may be a wait time we have to wait. The wait time may be provided
                    // by the query of the uri or its defined directly on the request object
                    auto waitStr = getOptionValue("wait", "0");
                    if (!waitStr.empty())
                        waitTime = std::stoi(waitStr);
                    if (waitTime)
                        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

                }
                catch (Raumkernel::Exception::RaumkernelException &e)
                {
                    if (e.type() == Raumkernel::Exception::ExceptionType::EXCEPTIONTYPE_APPCRASH)
                        throw e;
                }
                catch (std::exception &e)
                {
                    logError(e.what(), CURRENT_POSITION);
                }
                catch (std::string &e)
                {
                    logError(e, CURRENT_POSITION);
                }
                catch (OpenHome::Exception &e)
                {
                    logError(e.Message(), CURRENT_POSITION);;
                }
                catch (...)
                {
                    logError("Unknown exception!", CURRENT_POSITION);
                }
            }
            else
            {                
                logError("Invalid request options! Please validate path and query keys and values!", CURRENT_FUNCTION);
            }
            return ret;
        }


        bool RequestAction::executeAction()
        {
            // Overwrite!            
            return true;
        }


        std::string RequestAction::requestActionTypeToString(RequestActionType _requestActionType)
        {            
            if (_requestActionType == RequestActionType::RAA_NEXT) return "NEXT";
            if (_requestActionType == RequestActionType::RAA_PAUSE) return "PAUSE";
            if (_requestActionType == RequestActionType::RAA_PLAY) return "PLAY";
            if (_requestActionType == RequestActionType::RAA_PREV) return "PREV";
            if (_requestActionType == RequestActionType::RAA_SETVOLUME) return "SETVOLUME";
            if (_requestActionType == RequestActionType::RAA_STOP) return "STOP";
            if (_requestActionType == RequestActionType::RAA_VOLUMEDOWN) return "VOLUMEDOWN";
            if (_requestActionType == RequestActionType::RAA_VOLUMEUP) return "VOLUMEUP";
            if (_requestActionType == RequestActionType::RAA_VOLUMECHANGE) return "VOLUMECHANGE";
            if (_requestActionType == RequestActionType::RAA_CREATEZONE) return "CREATEZONE";
            if (_requestActionType == RequestActionType::RAA_ADDTOZONE) return "ADDTOZONE";
            if (_requestActionType == RequestActionType::RAA_DROPFROMZONE) return "DROPFROMZONE";
            if (_requestActionType == RequestActionType::RAA_MUTE) return "MUTE";
            if (_requestActionType == RequestActionType::RAA_UNMUTE) return "UNMUTE";
            if (_requestActionType == RequestActionType::RAA_SETPLAYMODE) return "SETPLAYMODE";
            if (_requestActionType == RequestActionType::RAA_LOADPLAYLIST) return "LOADPLAYLIST";
            if (_requestActionType == RequestActionType::RAA_LOADCONTAINER) return "LOADCONTAINER";
            if (_requestActionType == RequestActionType::RAA_LOADURI) return "LOADURI";
            if (_requestActionType == RequestActionType::RAA_SEEK) return "SEEK";
            if (_requestActionType == RequestActionType::RAA_FADETOVOLUME) return "FADETOVOLUME";
            if (_requestActionType == RequestActionType::RAA_TOGGLEMUTE) return "TOGGLEMUTE";
            if (_requestActionType == RequestActionType::RAA_SLEEPTIMER) return "SLEEPTIMER";
            if (_requestActionType == RequestActionType::RAA_SEEKTOTRACK) return "SEEKTOTRACK";
            if (_requestActionType == RequestActionType::RAA_LOADSHUFFLE) return "LOADSHUFFLE";
            if (_requestActionType == RequestActionType::RAA_ENTERAUTOMATICSTANDBY) return "ENTERAUTOMATICSTANDBY";
            if (_requestActionType == RequestActionType::RAA_ENTERMANUALSTANDBY) return "ENTERMANUALSTANDBY";
            if (_requestActionType == RequestActionType::RAA_LEAVESTANDBY) return "LEAVESTANDBY";
            if (_requestActionType == RequestActionType::RAA_CRASH) return "CRASH";

            // Returnable requests
            if (_requestActionType == RequestActionType::RAA_GETVERSION) return "GETVERSION";

            // Returnable requests with long polling ability
            if (_requestActionType == RequestActionType::RAA_GETZONECONFIG) return "GETZONECONFIG";
            if (_requestActionType == RequestActionType::RAA_GETMEDIALIST) return "GETMEDIALIST";
            if (_requestActionType == RequestActionType::RAA_GETZONEMEDIALIST) return "GETZONEMEDIALIST";
            if (_requestActionType == RequestActionType::RAA_GETRENDERERSTATE) return "GETRENDERERSTATE";
            if (_requestActionType == RequestActionType::RAA_GETRENDERERTRANSPORTSTATE) return "GETRENDERERTRANSPORTSTATE";            
            if (_requestActionType == RequestActionType::RAA_KILLSESSION) return "KILLSESSION";

            return "";
        }


        RequestActionType RequestAction::stringToRequestActionType(std::string _requestActionTypeString)
        {
            _requestActionTypeString = Raumkernel::Tools::StringUtil::toupper(_requestActionTypeString);
            if (_requestActionTypeString == "NEXT") return RequestActionType::RAA_NEXT;
            if (_requestActionTypeString == "PAUSE") return RequestActionType::RAA_PAUSE;
            if (_requestActionTypeString == "PLAY") return RequestActionType::RAA_PLAY;
            if (_requestActionTypeString == "PREV") return RequestActionType::RAA_PREV;
            if (_requestActionTypeString == "SETVOLUME") return RequestActionType::RAA_SETVOLUME;
            if (_requestActionTypeString == "STOP") return RequestActionType::RAA_STOP;
            if (_requestActionTypeString == "VOLUMEDOWN") return RequestActionType::RAA_VOLUMEDOWN;
            if (_requestActionTypeString == "VOLUMEUP") return RequestActionType::RAA_VOLUMEUP;  
            if (_requestActionTypeString == "VOLUMECHANGE") return RequestActionType::RAA_VOLUMECHANGE;
            if (_requestActionTypeString == "CREATEZONE") return RequestActionType::RAA_CREATEZONE;
            if (_requestActionTypeString == "ADDTOZONE") return RequestActionType::RAA_ADDTOZONE;
            if (_requestActionTypeString == "DROPFROMZONE") return RequestActionType::RAA_DROPFROMZONE;
            if (_requestActionTypeString == "MUTE") return RequestActionType::RAA_MUTE;
            if (_requestActionTypeString == "UNMUTE") return RequestActionType::RAA_UNMUTE;
            if (_requestActionTypeString == "SETPLAYMODE") return RequestActionType::RAA_SETPLAYMODE;
            if (_requestActionTypeString == "LOADPLAYLIST") return RequestActionType::RAA_LOADPLAYLIST;
            if (_requestActionTypeString == "LOADCONTAINER") return RequestActionType::RAA_LOADCONTAINER;
            if (_requestActionTypeString == "LOADURI") return RequestActionType::RAA_LOADURI; 
            if (_requestActionTypeString == "SEEK") return RequestActionType::RAA_SEEK;
            if (_requestActionTypeString == "FADETOVOLUME") return RequestActionType::RAA_FADETOVOLUME;
            if (_requestActionTypeString == "TOGGLEMUTE") return RequestActionType::RAA_TOGGLEMUTE;
            if (_requestActionTypeString == "SLEEPTIMER") return RequestActionType::RAA_SLEEPTIMER;
            if (_requestActionTypeString == "SEEKTOTRACK") return RequestActionType::RAA_SEEKTOTRACK;
            if (_requestActionTypeString == "LOADSHUFFLE") return RequestActionType::RAA_LOADSHUFFLE;
            if (_requestActionTypeString == "ENTERAUTOMATICSTANDBY") return RequestActionType::RAA_ENTERAUTOMATICSTANDBY;
            if (_requestActionTypeString == "ENTERMANUALSTANDBY") return RequestActionType::RAA_ENTERMANUALSTANDBY;
            if (_requestActionTypeString == "LEAVESTANDBY") return RequestActionType::RAA_LEAVESTANDBY;
            if (_requestActionTypeString == "CRASH") return RequestActionType::RAA_CRASH;

            // Returnable requests
            if (_requestActionTypeString == "GETVERSION") return RequestActionType::RAA_GETVERSION;       

            // Returnable requests with long polling ability
            if (_requestActionTypeString == "GETZONECONFIG") return RequestActionType::RAA_GETZONECONFIG;
            if (_requestActionTypeString == "GETMEDIALIST") return RequestActionType::RAA_GETMEDIALIST;
            if (_requestActionTypeString == "GETZONEMEDIALIST") return RequestActionType::RAA_GETZONEMEDIALIST;
            if (_requestActionTypeString == "GETRENDERERSTATE") return RequestActionType::RAA_GETRENDERERSTATE;
            if (_requestActionTypeString == "GETRENDERERTRANSPORTSTATE") return RequestActionType::RAA_GETRENDERERTRANSPORTSTATE;
            if (_requestActionTypeString == "KILLSESSION") return RequestActionType::RAA_KILLSESSION;

            return RequestActionType::RAA_UNDEFINED;
        }


        std::string RequestAction::getRequestInfo()
        {
            std::string options;
            for (auto it : requestOptions)
            {
                if (!options.empty())
                    options += " | ";
                options += it.first + ": " + it.second;
            }
            return RequestAction::requestActionTypeToString(action) + " / " + options;
        }


        std::shared_ptr<RequestAction> RequestAction::createFromUrl(std::string _url)
        {
            auto url = LUrlParser::clParseURL::ParseURL(_url);
            return RequestAction::createFromPath(url.m_Path, url.m_Query);
        }


        std::string RequestAction::getErrors()
        {
            return error;
        }


        bool RequestAction::isZoneScope(const std::string &_scope)
        {
            if (Raumkernel::Tools::StringUtil::tolower(_scope) == "zone")
                return true;
            return false;
        }


        std::shared_ptr<RequestAction> RequestAction::createFromPath(std::string _path, std::string _queryString)
        {          
            auto pathParts = Raumkernel::Tools::StringUtil::explodeString(_path, "/");
            // first path has to be empty, the second "raumserver" and the third path has to be "controller"
            // the third one should be the action. if there is another pathPart than we do have a problem
            if (pathParts.size() != 4)
                return nullptr;

            std::string action = pathParts[3];

            auto actionType = RequestAction::stringToRequestActionType(action);

            switch (actionType)
            {
                case RequestActionType::RAA_UNDEFINED: return nullptr;
                case RequestActionType::RAA_NEXT: return std::shared_ptr<RequestAction_Next>(new RequestAction_Next(_path, _queryString));
                case RequestActionType::RAA_PAUSE: return std::shared_ptr<RequestAction_Pause>(new RequestAction_Pause(_path, _queryString));
                case RequestActionType::RAA_PLAY: return std::shared_ptr<RequestAction_Play>(new RequestAction_Play(_path, _queryString));
                case RequestActionType::RAA_PREV: return std::shared_ptr<RequestAction_Prev>(new RequestAction_Prev(_path, _queryString));               
                case RequestActionType::RAA_STOP: return std::shared_ptr<RequestAction_Stop>(new RequestAction_Stop(_path, _queryString));
                case RequestActionType::RAA_SETVOLUME: return std::shared_ptr<RequestAction_SetVolume>(new RequestAction_SetVolume(_path, _queryString));
                case RequestActionType::RAA_VOLUMEDOWN: return std::shared_ptr<RequestAction_VolumeDown>(new RequestAction_VolumeDown(_path, _queryString));
                case RequestActionType::RAA_VOLUMEUP: return std::shared_ptr<RequestAction_VolumeUp>(new RequestAction_VolumeUp(_path, _queryString));                
                case RequestActionType::RAA_CREATEZONE: return std::shared_ptr<RequestAction_CreateZone>(new RequestAction_CreateZone(_path, _queryString));
                case RequestActionType::RAA_ADDTOZONE: return std::shared_ptr<RequestAction_AddToZone>(new RequestAction_AddToZone(_path, _queryString));
                case RequestActionType::RAA_DROPFROMZONE: return std::shared_ptr<RequestAction_DropFromZone>(new RequestAction_DropFromZone(_path, _queryString));
                case RequestActionType::RAA_MUTE: return std::shared_ptr<RequestAction_Mute>(new RequestAction_Mute(_path, _queryString));
                case RequestActionType::RAA_UNMUTE: return std::shared_ptr<RequestAction_Unmute>(new RequestAction_Unmute(_path, _queryString));
                case RequestActionType::RAA_SETPLAYMODE: return std::shared_ptr<RequestAction_SetPlayMode>(new RequestAction_SetPlayMode(_path, _queryString));
                case RequestActionType::RAA_LOADPLAYLIST: return std::shared_ptr<RequestAction_LoadPlaylist>(new RequestAction_LoadPlaylist(_path, _queryString));
                case RequestActionType::RAA_LOADCONTAINER: return std::shared_ptr<RequestAction_LoadContainer>(new RequestAction_LoadContainer(_path, _queryString));
                case RequestActionType::RAA_LOADURI: return std::shared_ptr<RequestAction_LoadUri>(new RequestAction_LoadUri(_path, _queryString));
                case RequestActionType::RAA_SEEK: return std::shared_ptr<RequestAction_Seek>(new RequestAction_Seek(_path, _queryString));
                case RequestActionType::RAA_SEEKTOTRACK:return std::shared_ptr<RequestAction_SeekToTrack>(new RequestAction_SeekToTrack(_path, _queryString));;
                case RequestActionType::RAA_FADETOVOLUME: return std::shared_ptr<RequestAction_FadeToVolume>(new RequestAction_FadeToVolume(_path, _queryString));
                case RequestActionType::RAA_TOGGLEMUTE: return std::shared_ptr<RequestAction_ToggleMute>(new RequestAction_ToggleMute(_path, _queryString));
                case RequestActionType::RAA_SLEEPTIMER: return std::shared_ptr<RequestAction_SleepTimer>(new RequestAction_SleepTimer(_path, _queryString));
                case RequestActionType::RAA_LOADSHUFFLE: return std::shared_ptr<RequestAction_LoadShuffle>(new RequestAction_LoadShuffle(_path, _queryString));
                case RequestActionType::RAA_ENTERAUTOMATICSTANDBY: return std::shared_ptr<RequestAction_EnterAutomaticStandby>(new RequestAction_EnterAutomaticStandby(_path, _queryString));
                case RequestActionType::RAA_ENTERMANUALSTANDBY: return std::shared_ptr<RequestAction_EnterManualStandby>(new RequestAction_EnterManualStandby(_path, _queryString));
                case RequestActionType::RAA_LEAVESTANDBY: return std::shared_ptr<RequestAction_LeaveStandby>(new RequestAction_LeaveStandby(_path, _queryString));
                case RequestActionType::RAA_CRASH: return std::shared_ptr<RequestAction_Crash>(new RequestAction_Crash(_path, _queryString));
                case RequestActionType::RAA_KILLSESSION: return std::shared_ptr<RequestAction_KillSession>(new RequestAction_KillSession(_path, _queryString));

                 // Returnable requests 
                case RequestActionType::RAA_GETVERSION: return std::shared_ptr<RequestActionReturnable_GetVersion>(new RequestActionReturnable_GetVersion(_path, _queryString));

                // Returnable requests with long polling ability
                case RequestActionType::RAA_GETZONECONFIG: return std::shared_ptr<RequestActionReturnableLongPolling_GetZoneConfig>(new RequestActionReturnableLongPolling_GetZoneConfig(_path, _queryString));
                case RequestActionType::RAA_GETMEDIALIST: return std::shared_ptr<RequestActionReturnableLongPolling_GetMediaList>(new RequestActionReturnableLongPolling_GetMediaList(_path, _queryString));
                case RequestActionType::RAA_GETZONEMEDIALIST: return std::shared_ptr<RequestActionReturnableLongPolling_GetZoneMediaList>(new RequestActionReturnableLongPolling_GetZoneMediaList(_path, _queryString));
                case RequestActionType::RAA_GETRENDERERSTATE: return std::shared_ptr<RequestActionReturnableLongPolling_GetRendererState>(new RequestActionReturnableLongPolling_GetRendererState(_path, _queryString));
                case RequestActionType::RAA_GETRENDERERTRANSPORTSTATE: return std::shared_ptr<RequestActionReturnableLongPolling_GetRendererTransportState>(new RequestActionReturnableLongPolling_GetRendererTransportState(_path, _queryString));
            }
            

            return nullptr;
        }
        

    }
}
