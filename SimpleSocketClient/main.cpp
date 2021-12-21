#include "main.h"

int main() {
    SOCKADDR_IN target;
    bzero(&target, sizeof(target));
    inet_pton(AF_INET, CONNECT_IP_ADDRESS, &(target.sin_addr));
    target.sin_family = AF_INET;
    target.sin_port = htons(CONNECT_PORT);

    client = connectServer(target);
    if (client == INVALID_SOCKET) {
        return EXIT_FAILURE;
    }
    session = new Session(client, target, handlePacket);

    int	threadCreateResult = pthread_create(&applicationThreadHandle, NULL, uiThread, NULL);
    if (threadCreateResult != 0) {
        cerr << "應用執行緒建立失敗" << endl;
        disconnect();
        return EXIT_FAILURE;
    }

    pthread_t receiveThreadHandle;
    threadCreateResult = pthread_create(&receiveThreadHandle, NULL, receiveThread, NULL);
    if (threadCreateResult != 0) {
        stopApplicationThread();
        system("clear");
        system("echo 收包執行緒建立失敗");
        return EXIT_FAILURE;
    }

    pthread_join(applicationThreadHandle, NULL);
    disconnect();
    pthread_join(receiveThreadHandle, NULL);
    return EXIT_SUCCESS;
}

void waitResponse() {
    unique_lock<mutex> lock(response);
    responseCondition.wait(lock, [] {
        return hasNotify;
    });
    hasNotify = false;
}

int parseInt(string value) {
    stringstream ss;
    ss << value;

    int ret;
    if ((ss >> ret).fail()) {
        throw runtime_error("輸入數值無效");
    }
    return ret;
}

bool isFileExist(string path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0 && (buffer.st_mode & S_IFMT) == S_IFREG;
}

string getFileName(string path) {
    filesystem::path filePath(path);
    return filePath.filename().string();
}

LPVOID uiThread(LPVOID lpParameter) {
    while (true) {
        system("clear");
        cout << "===== Simple Winsock Client =====" << endl;
        cout << GET_INFO << " 顯示伺服器資訊" << endl;
        cout << GET_AVATAR << " 下載伺服器圖標" << endl;
        cout << FILE_UPLOAD << " 上傳檔案" << endl;
        cout << GET_RANDOM_NUMBER << " 取得隨機數值" << endl;
        cout << ECHO << " 訊息回傳功能" << endl;
        cout << DISCONNECT << " 關閉連線" << endl;
        cout << "===== Simple Winsock Client =====" << endl << endl;

        string input;
        cout << "請選擇功能 : ";
        getline(cin, input);
        cout << endl;

        int value;
        try {
            value = parseInt(input);
        } catch (exception& ex) {
            cerr << ex.what() << endl;
            PAUSE
            continue;
        }
        if (value < GET_INFO || value > DISCONNECT) {
            cerr << "輸入數值範圍錯誤" << endl;
            PAUSE
            continue;
        }

        PacketLittleEndianWriter writer;
        writer.writeShort(value);

        switch (value) {
        case GET_INFO:
        case GET_AVATAR:
        case GET_RANDOM_NUMBER: {
            sendPacket(session, writer);
            waitResponse();
            break;
        }
        case FILE_UPLOAD: {
            cout << "請輸入檔案路徑 : ";
            getline(cin, input);
            cout << endl;

            if (!isFileExist(input)) {
                cerr << "檔案不存在" << endl;
                break;
            }

            string fileName;
            try {
                fileName = getFileName(input);
            } catch (exception& ex) {
                cerr << "取得檔案名稱時發生例外狀況 : " << ex.what() << endl;
                break;
            }

            writer.writeLengthAsciiString(fileName);
            writer.writeFile(input);
            sendPacket(session, writer);
            cout << "檔案已傳送" << endl;
            break;
        }
        case ECHO: {
            cout << "請輸入發送訊息 : ";
            getline(cin, input);
            cout << endl;

            writer.writeLengthAsciiString(input);
            sendPacket(session, writer);
            waitResponse();
            break;
        }
        case DISCONNECT: {
            return EXIT_SUCCESS;
        }
        }

        cout << endl;
        PAUSE
    }
    return EXIT_SUCCESS;
}
