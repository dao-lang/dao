#pragma once

#include "DaoParserBaseListener.h"

namespace dao {
    class DaoListener : public DaoParserBaseListener {
    public:
        void enterFile_input(DaoParser::File_inputContext *ctx) override {
            printf("ctx\n");
        }
    };
}