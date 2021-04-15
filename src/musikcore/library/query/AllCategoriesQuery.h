//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2021 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <musikcore/support/DeleteDefaults.h>
#include <musikcore/library/QueryBase.h>
#include <musikcore/library/query/util/SdkWrappers.h>
#include <musikcore/sdk/IValueList.h>

namespace musik { namespace core { namespace library { namespace query {

    class AllCategoriesQuery : public musik::core::library::query::QueryBase {
        public:
            static const std::string kQueryName;

            using Result = SdkValueList::Shared;

            DELETE_COPY_AND_ASSIGNMENT_DEFAULTS(AllCategoriesQuery)

            AllCategoriesQuery();

            virtual Result GetResult() noexcept;
            musik::core::sdk::IValueList* GetSdkResult();

            /* IQuery */
            std::string Name() override { return kQueryName; }

            /* ISerializableQuery */
            std::string SerializeQuery() override;
            std::string SerializeResult() override;
            void DeserializeResult(const std::string& data) override;
            static std::shared_ptr<AllCategoriesQuery> DeserializeQuery(const std::string& data);

        protected:
            /* QueryBase */
            bool OnRun(musik::core::db::Connection &db) override;

        private:
            Result result;
    };

} } } }
