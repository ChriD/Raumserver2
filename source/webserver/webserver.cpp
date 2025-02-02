
#include <raumserver/webserver/webserver.h>

namespace Raumserver
{
    namespace Server
    {

        Webserver::Webserver() : RaumserverBaseMgr()
        {
            serverObject = nullptr;
            isStarted = false;
            docroot = DOCUMENT_ROOT;
        }


        Webserver::~Webserver()
        {
            stop();
        }


        void Webserver::setDocumentRoot(std::string _docroot)
        {
            docroot = _docroot;
        }

     
        void Webserver::start(std::uint32_t _port)
        {
            if (isStarted)
                return;

            std::vector<std::string> serverOptions;

            try
            {
                logDebug("Starting webserver for requests on port: " + std::to_string(_port), CURRENT_POSITION);

                serverOptions.push_back("listening_ports");
                serverOptions.push_back(std::to_string(_port));
                
                serverOptions.push_back("document_root");
                serverOptions.push_back(docroot);

                serverOptions.push_back("error_log_file");
                serverOptions.push_back("error.log");

                serverObject = std::shared_ptr<CivetServer>(new CivetServer(serverOptions));              

                // add a general handler for the raumserver room and zone action handlings (like removing from zone or add to zone or room volumes, room mutes, aso...)
                serverRequestHandlerController = std::shared_ptr<RequestHandlerController>(new RequestHandlerController());
                serverRequestHandlerController->setManagerEngineerServer(getManagerEngineerServer());
                serverRequestHandlerController->setManagerEngineerKernel(getManagerEngineer());
                serverRequestHandlerController->setLogObject(getLogObject());
                serverObject->addHandler("/raumserver/controller", serverRequestHandlerController.get());                

                // add a general handler for fetching data 
                serverRequestHandlerData = std::shared_ptr<RequestHandlerData>(new RequestHandlerData());
                serverRequestHandlerData->setManagerEngineerServer(getManagerEngineerServer());
                serverRequestHandlerData->setManagerEngineerKernel(getManagerEngineer());
                serverRequestHandlerData->setLogObject(getLogObject());
                serverObject->addHandler("/raumserver/data", serverRequestHandlerData.get());

                                                         
                logInfo("Webserver for requests started (Port: " + std::to_string(_port) + ")", CURRENT_POSITION);
                isStarted = true;
            }    
            // errors when starting the server should lead to apprash!
            catch (Raumkernel::Exception::RaumkernelException &e)
            {          
                logError(e.what(), CURRENT_FUNCTION);
                throw e;
            }
            catch (CivetException &e)
            {
                logError(e.what(), CURRENT_FUNCTION);
                throw e;
            }
            catch (std::exception &e)
            {
                logError(e.what(), CURRENT_POSITION);
                throw e;
            }
            catch (std::string &e)
            {
                logError(e, CURRENT_POSITION);
                throw e;
            }            
            catch (...)
            {
                logError("Unknown exception!", CURRENT_POSITION);
                throw std::runtime_error("Unknown exception!");
            }         
        }


        void Webserver::stop()
        {            
            if (serverObject && isStarted)
                serverObject->close();
        }

   

        void RequestHandlerBase::setManagerEngineerServer(std::shared_ptr<Manager::ManagerEngineerServer> _managerEngineerServer)
        {
            managerEngineerServer = _managerEngineerServer;
        }

        std::shared_ptr<Manager::ManagerEngineerServer> RequestHandlerBase::getManagerEngineerServer()
        {
            return managerEngineerServer;
        }

        void RequestHandlerBase::setManagerEngineerKernel(std::shared_ptr<Raumkernel::Manager::ManagerEngineer> _managerEngineerKernel)
        {
            managerEngineerKernel = _managerEngineerKernel;
        }

        std::shared_ptr<Raumkernel::Manager::ManagerEngineer> RequestHandlerBase::getManagerEngineerKernel()
        {
            return managerEngineerKernel;
        }

        void RequestHandlerBase::setLogObject(std::shared_ptr<Raumkernel::Log::Log> _logObject)
        {
            logObject = _logObject;
        }

        std::shared_ptr<Raumkernel::Log::Log> RequestHandlerBase::getLogObject()
        {
            return logObject;
        }


        std::string RequestHandlerBase::buildCorsHeader(std::map<std::string, std::string>* _headerVars)
        {
            std::string corsHeader = "Access-Control-Allow-Origin: *";  
            std::string headerVarListInp = "sessionId,updateId";
            std::string headerVarListExp = "sessionId,updateId";

            if (_headerVars && _headerVars->size())
            {                
                for (auto it = _headerVars->begin(); it != _headerVars->end(); it++)
                //for (auto pair : *_headerVars)
                {
                    headerVarListInp += "," +  it->first;
                }
                //headerVarListInp.pop_back();
                headerVarListExp = headerVarListInp;
            }
            else
            {
               // headerVarListInp = "*";
               // headerVarListExp = "*";
            }

            corsHeader += "\r\nAccess-Control-Allow-Headers: " + headerVarListInp;
            corsHeader += "\r\nAccess-Control-Expose-Headers: " + headerVarListExp;           
            return corsHeader;
        }

        void RequestHandlerBase::sendResponse(struct mg_connection *_conn, std::string _string, bool _error, Request::RequestAction * _reqAction)
        {    
            /*
            mg_printf(_conn, std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n" + buildCorsHeader() + "\r\n\r\n").c_str());
            mg_printf(_conn, "<html><body>\r\n");
            mg_printf(_conn, "<h2>Raumserver</h2>\r\n");
            mg_printf(_conn, _string.c_str());
            mg_printf(_conn, "</body></html>\r\n");
            */

            const struct mg_request_info *request_info = mg_get_request_info(_conn);
            
            rapidjson::StringBuffer jsonStringBuffer;
            rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStringBuffer);

            jsonWriter.StartObject();
            if (request_info->request_uri)
            {
                jsonWriter.Key("requestUrl");   jsonWriter.String(request_info->request_uri);
            }
            else
            {
                jsonWriter.Key("requestUrl");   jsonWriter.String("");
            }
            if (request_info->query_string)
            {
                jsonWriter.Key("requestQuery"); jsonWriter.String(request_info->query_string);
            }
            else
            {
                jsonWriter.Key("requestQuery"); jsonWriter.String("");
            }
            if (_reqAction != nullptr)
            {
                jsonWriter.Key("action");  jsonWriter.String(Request::RequestAction::requestActionTypeToString(_reqAction->getActionType()).c_str());
            }
            else
            {
                jsonWriter.Key("action");  jsonWriter.String("");
            }
            jsonWriter.Key("msg");          jsonWriter.String(_string.c_str());
            jsonWriter.Key("error");        jsonWriter.Bool(_error);
            jsonWriter.EndObject();

            std::string reqReturn = jsonStringBuffer.GetString();
            mg_printf(_conn, reqReturn.c_str());                        
        }


        void RequestHandlerBase::sendDataResponse(struct mg_connection *_conn, std::string _string, std::map<std::string, std::string> _headerVars, bool _error, Request::RequestAction * _reqAction)
        {       
            // create header string
            std::string headers = "";
            for (auto pair : _headerVars)
            {
                headers += pair.first + ":" + pair.second + "\r\n";
            }

            mg_printf(_conn, std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n" + buildCorsHeader(&_headerVars) + "\r\n" + headers + "Connection: close\r\n\r\n").c_str());
            mg_printf(_conn, _string.c_str());         
        }


        bool RequestHandlerController::handleGet(CivetServer *_server, struct mg_connection *_conn)
        {
            // Check if system is online, otherwise don't execute!
            if (!getManagerEngineerServer() || !getManagerEngineerServer()->isSystemReady())
            {
                sendResponse(_conn, "Raumfeld System is not ready to receive requests!", true);
                return true;
            }

            const struct mg_request_info *request_info = mg_get_request_info(_conn);   

            // create request action object from url given from the connection
            std::shared_ptr<Request::RequestAction> requestAction = Request::RequestAction::createFromPath(request_info->request_uri, request_info->query_string == nullptr ? "" : request_info->query_string);

            // if there is no reuquest action the path is wrong or not existent!
            if (!requestAction)
            {
                sendResponse(_conn, "Action for request '" + std::string(request_info->request_uri) + "' not found! Please check to the documentation for valid requests!", true, requestAction.get());
                return true;                
            }

            requestAction->setManagerEngineer(getManagerEngineerKernel());
            requestAction->setManagerEngineerServer(getManagerEngineerServer());
            requestAction->setLogObject(getLogObject());

            // if we should stack the request we have to add it to the request manager and return the error values of the validate if there are some
            // the Reuest-Manager will take care of the Request from now on
            if (requestAction->isStackable())
            {
                if (requestAction->isValid())
                {
                    getManagerEngineerServer()->getRequestActionManager()->addRequestAction(requestAction);                    
                    sendResponse(_conn, "Request '" + std::string(request_info->request_uri) + "' was added to queue!", false, requestAction.get());
                }
                else
                {                    
                    sendResponse(_conn, "Error while executing request: '" + requestAction->getErrors(), true, requestAction.get());
                }
            }
            // the request is not stackable, that means we have to execute it right now
            // if the request is a returnable item we have to return the data string from the requestAction
            else
            {
                // a returnable request ist always a sync and non stackable request
                if (std::dynamic_pointer_cast<Request::RequestActionReturnable>(requestAction))
                {
                    if (requestAction->execute())
                    {
                        auto requestActionReturnable = std::dynamic_pointer_cast<Request::RequestActionReturnable>(requestAction);
                        sendDataResponse(_conn, requestActionReturnable->getResponseData(), requestActionReturnable->getResponseHeader(), false, requestAction.get());
                    }
                    else
                    {
                        // TODO: set better response!
                        sendDataResponse(_conn, "ERROR'" + requestAction->getErrors(), std::map<std::string, std::string>(), true, requestAction.get());
                    }
                    
                }
                else
                {
                    requestAction->execute();
                    sendResponse(_conn, "Request '" + std::string(request_info->request_uri) + "' was executed!", false);                    
                }
            }
             
            return true;
        }


        bool RequestHandlerController::handleOptions(CivetServer *_server, struct mg_connection *_conn)
        {
            return handleGet(_server, _conn);
        }


        bool RequestHandlerData::handleGet(CivetServer *_server, struct mg_connection *_conn)
        {
            return RequestHandlerController::handleGet(_server, _conn);
        }


        bool RequestHandlerData::handleOptions(CivetServer *_server, struct mg_connection *_conn)
        {
            return RequestHandlerController::handleOptions(_server, _conn);
        }
   
    }
}