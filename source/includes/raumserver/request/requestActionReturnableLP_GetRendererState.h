//
// The MIT License (MIT)
//
// Copyright (c) 2016 by ChriD
//
// Permission is hereby granted, free of charge,  to any person obtaining a copy of
// this software and  associated documentation  files  (the "Software"), to deal in
// the  Software  without  restriction,  including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software,  and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this  permission notice  shall be included in all
// copies or substantial portions of the Software.
//
// THE  SOFTWARE  IS  PROVIDED  "AS IS",  WITHOUT  WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE  LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER
// IN  AN  ACTION  OF  CONTRACT,  TORT  OR  OTHERWISE,  ARISING  FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once
#ifndef RAUMSERVER_REQUESTACTIONRETURNABLE_LP_GETRENDERERSTATE_H
#define RAUMSERVER_REQUESTACTIONRETURNABLE_LP_GETRENDERERSTATE_H

#include <raumserver/request/requestActionReturnableLP.h>

namespace Raumserver
{
    namespace Request
    {
        class RequestActionReturnableLongPolling_GetRendererState : public RequestActionReturnableLongPolling
        {
            public:
                EXPORT RequestActionReturnableLongPolling_GetRendererState(std::string _url);
                EXPORT RequestActionReturnableLongPolling_GetRendererState(std::string _path, std::string _query);
                EXPORT virtual ~RequestActionReturnableLongPolling_GetRendererState();
                EXPORT virtual bool isValid() override;
                EXPORT virtual bool executeActionLongPolling() override;                  

            protected:
                virtual std::string getLastUpdateId() override;                       
                void addRendererStateToJson(const std::string &_zoneUDN, Raumkernel::Devices::MediaRendererState &_rendererState, Raumkernel::Devices::MediaRenderer* _mediaRenderer, rapidjson::Writer<rapidjson::StringBuffer> &_jsonWriter);                                
        };
    }
}


#endif