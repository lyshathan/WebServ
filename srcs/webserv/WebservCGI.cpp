#include "Webserv.hpp"
#include "../parsing/Client.hpp"

void Webserv::handleCGIWrite(int clientFd, CgiState *cgiState) {
    size_t remaining = cgiState->request_body.size() - cgiState->bytes_written;

    if (remaining <= 0) {
        closeCGIStdin(cgiState);
        return;
    }

    ssize_t written = write(cgiState->stdin_fd,
                        cgiState->request_body.c_str() + cgiState->bytes_written,
                        remaining);

    if (written > 0) {
        cgiState->bytes_written += written;
        if (cgiState->bytes_written >= cgiState->request_body.size()) {
            closeCGIStdin(cgiState);
        }
    } else {
        _clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
        cleanupCGI(clientFd, cgiState);
        processAndSendResponse(clientFd);
    }
}

void Webserv::handleCGIRead(int clientFd, CgiState *cgiState) {
    char buffer[4096];
    ssize_t bytesRead = read(cgiState->stdout_fd, buffer, sizeof(buffer));

    if (bytesRead > 0) {
        cgiState->response_buffer.append(buffer, bytesRead);
        
        // // Check if we have complete headers and Content-Length
        // size_t contentLength;
        // if (hasCGIContentLength(cgiState->response_buffer, contentLength)) {
        //     // Find where body starts
        //     std::cout << "Content length " << contentLength << "\n";
        //     size_t headerEnd = cgiState->response_buffer.find("\r\n\r\n");
        //     if (headerEnd == std::string::npos) {
        //         headerEnd = cgiState->response_buffer.find("\n\n");
        //         if (headerEnd != std::string::npos) {
        //             headerEnd += 2;
        //         } else {
        //             return; // Headers not complete yet
        //         }
        //     } else {
        //         headerEnd += 4;
        //     }
            
        //     // Check if we have received the complete body
        //     size_t bodyReceived = cgiState->response_buffer.length() - headerEnd;
        //     if (bodyReceived >= contentLength) {
        //         // We have complete response, close and process
        //         close(cgiState->stdout_fd);
        //         cgiState->stdout_fd = -1;
        //         handleCGICompletion(clientFd, cgiState);
        //         return;
        //     }
        // }
        // // If no Content-Length found, continue reading until EOF
        
    } else if (bytesRead == 0) {
        close(cgiState->stdout_fd);
        cgiState->stdout_fd = -1;
        handleCGICompletion(clientFd, cgiState);
    } else {
        _clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
        cleanupCGI(clientFd, cgiState);
        processAndSendResponse(clientFd);
    }
}

void Webserv::handleCGICompletion(int clientFd, CgiState *cgiState) {
    int status;
    pid_t result = waitpid(cgiState->pid, &status, WNOHANG);

    if (result == cgiState->pid) {
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            _clients[clientFd]->httpReq->setCGIResult(cgiState->response_buffer);
            _clients[clientFd]->httpReq->setStatus(OK);
        } else {
            _clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
        }

        cleanupCGI(clientFd, cgiState);
        processAndSendResponse(clientFd);
    } else if (result == -1) {
        _clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
        cleanupCGI(clientFd, cgiState);
        processAndSendResponse(clientFd);
    }
}