/**
 * opencv and brpc test project
 */
#ifndef WRAPPER_SERVER_H_
#define WRAPPER_SERVER_H_

#include <butil/logging.h>
#include <butil/iobuf.h>
#include <butil/strings/string_piece.h>
#include <brpc/server.h>
#include "basic_server.pb.h"
#include "database.h"
#include "interfaces.h"
#include "config.h"

#define GET_NAME(x) #x

#define DECLARE_SERVER(SERVER_NAME, FUNC, ARGUMENT) \
    virtual void SERVER_NAME( \
        google::protobuf::RpcController * cntl_base, \
        const base_server::Request * request, \
        base_server::Response * response, \
        google::protobuf::Closure * done){ \
        LOG(INFO) << "start server ..."; \
        interfaces::Context ctx(0); \
        ctx.start(); \
        brpc::ClosureGuard done_guard(done); \
        brpc::Controller *cntl = static_cast < brpc::Controller * > (cntl_base); \
        base_server::Output *output = response->mutable_output(); \
        bool error_flag = 0; \
        std::string error_msg = ""; \
        try { \
            this->FUNC(request, output, ARGUMENT); \
            LOG(INFO) << "implement success!"; \
        } catch(std::runtime_error &error){ \
            error_msg = error.what(); \
            LOG(ERROR) << "error in server: " << error_msg; \
            error_flag = 1; \
        } \
        if (error_flag == 0) { \
            ctx.success(); \
        } else { \
            ctx.fail(error_msg); \
        } \
        ctx.finish(); \
        base_server::Context *_ctx = response->mutable_ctx(); \
        _ctx->set_session_id(ctx.session_id); \
        _ctx->set_status(ctx.status); \
        _ctx->set_message(ctx.message); \
        _ctx->set_start_time(ctx.start_time); \
        _ctx->set_end_time(ctx.end_time); \
        LOG(INFO) << "end server ..."; \
        LOG(INFO) << " "; \
    }


#define DECLARE_ECHO_SERVER(SERVER_NAME, FUNC) \
    virtual void SERVER_NAME( \
        google::protobuf::RpcController * cntl_base, \
        const base_server::Request * request, \
        base_server::Response * response, \
        google::protobuf::Closure * done){ \
        brpc::ClosureGuard done_guard(done); \
        brpc::Controller *cntl = static_cast < brpc::Controller * > (cntl_base); \
        base_server::Output *output = response->mutable_output(); \
        this->FUNC(request, output); \
        base_server::Context *_ctx = response->mutable_ctx(); \
        _ctx->set_session_id("0"); \
        _ctx->set_status("OK"); \
        _ctx->set_message(""); \
        _ctx->set_start_time(""); \
        _ctx->set_end_time(""); \
    }

#endif /* ifndef STATUS_SERVER_H_ */
