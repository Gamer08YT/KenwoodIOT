//
// Created by Jan Heil on 23.07.2023.
//

#ifndef KENWOODIOT_REMOTE_H
#define KENWOODIOT_REMOTE_H


class Remote {

public:
    static void send_cmd(int dataIO);

    static void prepare();

    static void handleIR();
};


#endif //KENWOODIOT_REMOTE_H
