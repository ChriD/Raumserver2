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
#ifndef RAUMSERVER_REQUESTACTION_CRASH_H
#define RAUMSERVER_REQUESTACTION_CRASH_H

#include <raumserver/request/requestAction.h>

namespace Raumserver
{
    namespace Request
    {
        class RequestAction_Crash : public RequestAction
        {
            public:                
                EXPORT RequestAction_Crash(std::string _url);
                EXPORT RequestAction_Crash(std::string _path, std::string _query);
                EXPORT virtual ~RequestAction_Crash();
                EXPORT virtual bool executeAction() override;
                EXPORT virtual bool isValid() override;

                EXPORT virtual void crashLevel1();
                EXPORT virtual void crashLevel2();
                EXPORT virtual void crashLevel3();
                EXPORT virtual void crashLevel4();

            protected:
        };
    }
}


#endif