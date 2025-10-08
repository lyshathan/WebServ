#include "Webserv.hpp"
#include "../parsing/Client.hpp"

void Webserv::addCGIToPoll(int clientFd) {
    CgiState* cgiState = _clients[clientFd]->httpReq->getCGIState();

    struct pollfd stdout_pollfd = {cgiState->stdout_fd, POLLIN, 0};
    _pollFds.push_back(stdout_pollfd);
    _cgiToClient[cgiState->stdout_fd] = clientFd;

    if (cgiState->state == CgiState::WRITING && cgiState->stdin_fd != -1) {
        struct pollfd stdin_pollfd = {cgiState->stdin_fd, POLLOUT, 0};
        _pollFds.push_back(stdin_pollfd);
        _cgiToClient[cgiState->stdin_fd] = clientFd;
    }
}

void Webserv::cleanupCGI(int clientFd, CgiState *cgiState) {
    if (cgiState->stdin_fd != -1) {
        close(cgiState->stdin_fd);
        removeFdFromPoll(cgiState->stdin_fd);
    }
    if (cgiState->stdout_fd != -1) {
        close(cgiState->stdout_fd);
        removeFdFromPoll(cgiState->stdout_fd);
    }

    if (cgiState->pid > 0) {
        kill(cgiState->pid, SIGTERM);
        waitpid(cgiState->pid, NULL, 0);
    }

    delete cgiState;
    _clients[clientFd]->httpReq->setCGIState(NULL);
}

void Webserv::closeCGIStdin(CgiState *cgiState) {
    if (cgiState->stdin_fd != -1) {
        close(cgiState->stdin_fd);
        removeFdFromPoll(cgiState->stdin_fd);
        cgiState->stdin_fd = -1;
    }
    cgiState->state = CgiState::READING;
}

void Webserv::removeFdFromPoll(int fd) {
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == fd) {
            _pollFds.erase(it);
            break;
        }
    }
    
    std::map<int, int>::iterator mapIt = _cgiToClient.find(fd);
    if (mapIt != _cgiToClient.end()) {
        _cgiToClient.erase(mapIt);
    }
}