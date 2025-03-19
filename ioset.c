#ifndef _IOSET_H_
    #define _IOSET_H_
    #include <assert.h>
    #include <complex.h>
    #include <ctype.h>
    #include <errno.h>
    #include <fenv.h>
    #include <float.h>
    #include <inttypes.h>
    #include <iso646.h>
    #include <limits.h>
    #include <locale.h>
    #include <math.h>
    #include <setjmp.h>
    #include <signal.h>
    #include <stdarg.h>
    #include <stdatomic.h>
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <tgmath.h>
    #include <time.h>
    #include <uchar.h>
    #include <wchar.h>
    #include <wctype.h>
    #include <pthread.h>

    #ifdef __linux__ 
        #include <unistd.h>
        #include <fcntl.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <sys/mman.h>
        #include <sys/wait.h>
        #include <sys/socket.h>
        #include <sys/un.h>
        #include <sys/ipc.h>
        #include <sys/msg.h>
        #include <sys/sem.h>
        #include <sys/shm.h>
        #include <sys/time.h>
        #include <sys/resource.h>
        #include <sys/utsname.h>
        #include <sys/ioctl.h>
        #include <arpa/inet.h>
        #include <netinet/in.h>
        #include <netinet/tcp.h>
        #include <dirent.h>
        #include <dlfcn.h>

    #elif _WIN32
        #include <Windows.h>
        #include <tchar.h>
        #include <winbase.h>
        #include <wingdi.h>
        #include <winnt.h>
        #include <commdlg.h>
        #include <shlobj.h>
        #include <WinSock2.h>
        #include <windowsx.h>
        #include <wininet.h>
        #include <ws2tcpip.h>
        #include <process.h>
        #include <conio.h>
        #include <direct.h>
        #include <io.h>
        #include <mmsystem.h>
        #include <objbase.h>
        #include <ole2.h>
        #include <rpc.h>
        #include <strsafe.h>
        #pragma comment(lib, "ws2_32.lib")
        #pragma warning(disable:4996)

    #else

    #endif


    #ifdef __linux__
        int sock_create(char *mode){
            int sock;
            if (strcmp(mode, "tcp") == 0){
                sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP); /* ソケットを作成 (TCP) */
                if (sock < 0) { 
                    perror("socket() failed");
                    return -1;
                }
            }
            else if (strcmp(mode, "udp") == 0){
                sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP); /* ソケットを作成する (UDP)*/
                if (sock < 0) { 
                    perror("socket() failed");
                    return -1;
                }
            }
            else{
                perror("socket() failed");
                return -1;
            }
            return sock;
        }
        int port_bind(int sock, struct sockaddr_in *addr){ /* ポートに結びつける */
            int addrSize = sizeof(*addr);
            if (bind(sock, (struct sockaddr *)addr, addrSize) < 0) { 
                perror("bind() failed");
                return 1;
            }
            return 0;
        }

        int udp_send(int sock, struct sockaddr_in *addr, char *msg){ /* 送信 */
            int len = strlen(msg);
            int addrSize = sizeof(*addr);
            if (sendto(sock, msg, len, 0, (struct sockaddr *)addr, addrSize) != len){
                perror("sendto() failed");
                return 1;
            }
            return 0;
        }

        int udp_recv(int sock, struct sockaddr_in *caddr, char* buff){ /* 受信する */
            int bytes;
            unsigned int csize = sizeof(*caddr);
            int bufsize = sizeof(*buff)/sizeof(buff[0]);
        
            bytes = recvfrom (sock, buff, bufsize - 1, 0, (struct sockaddr *)caddr, &csize);
            if ((bytes) < 0) { // 空文字をエラーにしない
                perror("recvfrom() failed");
                return 1;
            }
            buff[bytes] = '\0'; /* 文字列として扱うため最後を \0 で終端して表示する */ 
            return 0;
        }

        int port_connect(int sock, struct sockaddr_in *addr){ /* 接続する */
            if (connect(sock, (struct sockaddr *)addr, sizeof(*addr)) < 0) { 
                perror("connect() failed");
                return 1;
            }
            return 0;
        }

        int tcp_send(int sock, char *msg){ /* 送信する */
            int len = strlen(msg);
            if (send(sock, msg, len, 0) != len) {
                perror("send() failed");
                return 1;
            }
            return 0;
        }

        int port_listen(int sock, int maxpending){
            if (listen(sock, maxpending) < 0) {
                perror("listen() failed");
                return 1;
            }
            return 0;
        }

        int port_accept(int sock, struct sockaddr_in *caddr){
            int csock;
            unsigned int csize = sizeof(*caddr);
            csock = accept(sock, (struct sockaddr *)caddr, &csize);
            if (csock< 0) {
                perror ("accept() failed");
                return -1;
            }
            return csock;
        }

        int tcp_recv(int csock, char* buff){ /* 受信する */
            int bytes;
            int bufsize = sizeof(*buff)/sizeof(buff[0]);
            bytes = recv(csock, buff, bufsize - 1, 0);
            if (bytes < 0) { // 空文字をエラーにしない
                perror("recv() failed");
                return 1;
            }
            buff[bytes] = '\0'; /* 文字列として扱うため最後を \0 で終端して表示する */ 
            return 0;
        }

    #elif _WIN32
        #define OPEN_PROCESS_TOKEN      (TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY)
        WSADATA wsaData;
        int sock_create(char *mode){
            WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0) : winsock ver. 2.0
            int sock;
            if (strcmp(mode, "tcp") == 0){
                sock = socket (PF_INET, SOCK_STREAM, 0); /* ソケットを作成 (TCP) */
                if (sock < 0) { 
                    perror("socket() failed");
                    return -1;
                }
            }
            else if (strcmp(mode, "udp") == 0){
                sock = socket (PF_INET, SOCK_DGRAM, 0); /* ソケットを作成する (UDP)*/
                if (sock < 0) { 
                    perror("socket() failed");
                    return -1;
                }
            }
            else{
                perror("socket() failed");
                return -1;
            }
            return sock;
        }
        int close(int sock){
            closesocket(sock); 
            WSACleanup();
        }
        
        int port_bind(int sock, struct sockaddr_in *addr){ /* ポートに結びつける */
            int addrSize = sizeof(*addr);
            if (bind(sock, (struct sockaddr *)addr, addrSize) < 0) { 
                perror("bind() failed");
                return 1;
            }
            return 0;
        }

        int udp_send(int sock, struct sockaddr_in *addr, char *msg){ /* 送信 */
            int len = strlen(msg);
            int addrSize = sizeof(*addr);
            if (sendto(sock, msg, len, 0, (struct sockaddr *)addr, addrSize) != len){
                perror("sendto() failed");
                return 1;
            }
            return 0;
        }

        int udp_recv(int sock, struct sockaddr_in *caddr, char* buff){ /* 受信する */
            int bytes;
            unsigned int csize = sizeof(*caddr);
            int bufsize = sizeof(*buff)/sizeof(buff[0]);
        
            bytes = recvfrom (sock, buff, bufsize - 1, 0, (struct sockaddr *)caddr, &csize);
            if ((bytes) < 0) { // 空文字をエラーにしない
                perror("recvfrom() failed");
                return 1;
            }
            buff[bytes] = '\0'; /* 文字列として扱うため最後を \0 で終端して表示する */ 
            return 0;
        }

        int port_connect(int sock, struct sockaddr_in *addr){ /* 接続する */
            if (connect(sock, (struct sockaddr *)addr, sizeof(*addr)) < 0) { 
                perror("connect() failed");
                return 1;
            }
            return 0;
        }

        int tcp_send(int sock, char *msg){ /* 送信する */
            int len = strlen(msg);
            if (send(sock, msg, len, 0) != len) {
                perror("send() failed");
                return 1;
            }
            return 0;
        }

        int port_listen(int sock, int maxpending){
            if (listen(sock, maxpending) < 0) {
                perror("listen() failed");
                return 1;
            }
            return 0;
        }

        int port_accept(int sock, struct sockaddr_in *caddr){
            int csock;
            unsigned int csize = sizeof(*caddr);
            csock = accept(sock, (struct sockaddr *)caddr, &csize);
            if (csock< 0) {
                perror ("accept() failed");
                return -1;
            }
            return csock;
        }

        int tcp_recv(int csock, char* buff){ /* 受信する */
            int bytes;
            int bufsize = sizeof(*buff)/sizeof(buff[0]);
            bytes = recv(csock, buff, bufsize - 1, 0);
            if (bytes < 0) { // 空文字をエラーにしない
                perror("recv() failed");
                return 1;
            }
            buff[bytes] = '\0'; /* 文字列として扱うため最後を \0 で終端して表示する */ 
            return 0;
        }

    
        #define TRAY_H
        #define WM_TRAY_CALLBACK_MESSAGE (WM_USER + 1)
        #define WC_TRAY_CLASS_NAME "TRAY"
        #define ID_TRAY_FIRST 1000
        HMENU   trayMenuItem;
        struct tray_menu;
    
        struct tray {
            char *icon;        
            struct tray_menu *menu;
        };
    
        struct tray_menu {
            char *text;
            int disabled;
            int checked;
    
            void (*cb)(struct tray_menu *);
            void *context;
    
            struct tray_menu *submenu;
        };
    
        struct TaskTray{ //タスクトレイ
            WNDCLASSEX      wc;
            NOTIFYICONDATA  nid;
            HWND            hwnd;
            UINT            id;
        };
        TaskTray _taskTray;
        HMENU _tray_menu(struct tray_menu *m) {
            HMENU hMenu = CreatePopupMenu();
            for (; m != NULL && m->text != NULL; m++, (_taskTray.id)++) {
                if (strcmp(m->text, "-") == 0) {
                    InsertMenu(hMenu, _taskTray.id, MF_SEPARATOR, TRUE, "");
                } 
                else {
                    MENUITEMINFO item;
                    memset(&item, 0, sizeof(item));
                    item.cbSize = sizeof(MENUITEMINFO);
                    item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
                    item.fType = 0;
                    item.fState = 0;
                    if (m->submenu != NULL) {
                        item.fMask = item.fMask | MIIM_SUBMENU;
                        item.hSubMenu = _tray_menu(m->submenu);
                    }
                    if (m->disabled) {
                        item.fState |= MFS_DISABLED;
                    }
                    if (m->checked) {
                        item.fState |= MFS_CHECKED;
                    }
                    item.wID = _taskTray.id;
                    item.dwTypeData = m->text;
                    item.dwItemData = (ULONG_PTR)m;
                    InsertMenuItem(hMenu, _taskTray.id, TRUE, &item);
                }
            }
            return hMenu;
        }
        int tray_loop(int blocking) {
            MSG msg;
            if (blocking) {
                GetMessage(&msg, NULL, 0, 0);
            } 
            else {
                PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
            }
            if (msg.message == WM_QUIT) {
                return -1;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return 0;
        }
        void tray_update(struct tray *taskTray) {
            HMENU prevmenu = trayMenuItem;
            trayMenuItem = _tray_menu(taskTray->menu);
            SendMessage(_taskTray.hwnd, WM_INITMENUPOPUP, (WPARAM)trayMenuItem, 0);
            HICON icon;
            ExtractIconEx(taskTray->icon, 0, NULL, &icon, 1);
            if (_taskTray.nid.hIcon) {
                DestroyIcon(_taskTray.nid.hIcon);
            }
            _taskTray.nid.hIcon = icon;
            Shell_NotifyIcon(NIM_MODIFY, &nid);
            if (prevmenu != NULL) {
                DestroyMenu(prevmenu);
            }
        }
        TaskTray(std::string menuName) {
            _taskTray.id = ID_TRAY_FIRST;
            memset(&_taskTray.wc, 0, sizeof(_taskTray.wc));
            _taskTray.wc.cbSize = sizeof(WNDCLASSEX);
            _taskTray.wc.lpfnWndProc = (WNDPROC)_tray_wnd_proc;
            _taskTray.wc.hInstance = GetModuleHandle(NULL);
            _taskTray.wc.lpszClassName = WC_TRAY_CLASS_NAME;
            _taskTray.wc.lpszMenuName = menuName.c_str();
            if (!RegisterClassEx(&_taskTray.wc)) {
                return;
            }
            _taskTray.hwnd = CreateWindowEx(0, WC_TRAY_CLASS_NAME, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if (_taskTray.hwnd == NULL) {
                return;
            }
            UpdateWindow(_taskTray.hwnd);
            memset(&_taskTray.nid, 0, sizeof(_taskTray.nid));
            _taskTray.nid.cbSize = sizeof(NOTIFYICONDATA);
            _taskTray.nid.hWnd = _taskTray.hwnd;
            _taskTray.nid.uID = 0;
            _taskTray.nid.uFlags = NIF_ICON | NIF_MESSAGE;
            _taskTray.nid.uCallbackMessage = WM_TRAY_CALLBACK_MESSAGE;
            lstrcpy( _taskTray.nid.szTip, TEXT(menuName.c_str())); 
            Shell_NotifyIcon(NIM_ADD, &_taskTray.nid);
            return;
        }
        void tray_exit() {
            Shell_NotifyIcon(NIM_DELETE, &_taskTray.nid);
            if (_taskTray.nid.hIcon != 0) {
                DestroyIcon(_taskTray.nid.hIcon);
            }
            if (trayMenuItem != 0) {
                DestroyMenu(trayMenuItem);
            }
            PostQuitMessage(0);
            UnregisterClass(WC_TRAY_CLASS_NAME, GetModuleHandle(NULL));
        }
        LRESULT CALLBACK _tray_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
            switch (msg) {
                case WM_CLOSE:
                    DestroyWindow(hwnd);
                    return 0;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_TRAY_CALLBACK_MESSAGE:
                    switch (lparam){
                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                            POINT p;
                            GetCursorPos(&p);
                            SetForegroundWindow(hwnd);
                            WORD cmd = TrackPopupMenu(
                                trayMenuItem, TPM_LEFTALIGN | 
                                TPM_RIGHTBUTTON | 
                                TPM_RETURNCMD | 
                                TPM_NONOTIFY,
                                p.x, 
                                p.y, 
                                0, 
                                hwnd, 
                                NULL
                            );
                            SendMessage(hwnd, WM_COMMAND, cmd, 0);
                            return 0;
                    }
                    break;
                case WM_COMMAND:
                    if (wparam >= ID_TRAY_FIRST) {
                        MENUITEMINFO item = {
                            .cbSize = sizeof(MENUITEMINFO), .fMask = MIIM_ID | MIIM_DATA,
                        };
                        if (GetMenuItemInfo(trayMenuItem, wparam, FALSE, &item)) {
                            struct tray_menu *menu = (struct tray_menu *)item.dwItemData;
                            if (menu != NULL && menu->cb != NULL) {
                                menu->cb(menu);
                            }
                        }
                        return 0;
                    }
                    break;
            }
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
        // プロセスハンドルから特権名を有効/無効
        BOOL SetProcessPrivilege(HANDLE hProcess, LPCTSTR lpPrivilegeName, BOOL bEnable) {
            HANDLE hToken;
            LUID luid;
            TOKEN_PRIVILEGES tp;

            if (!OpenProcessToken(hProcess, OPEN_PROCESS_TOKEN, &hToken)) {
                return FALSE;
            }

            if (!LookupPrivilegeValue(NULL, lpPrivilegeName, &luid)) {
                CloseHandle(hToken);
                return FALSE;
            }

            tp.PrivilegeCount = 1;
            tp.Privileges[0].Luid = luid;
            tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

            BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL) && GetLastError() == ERROR_SUCCESS;

            CloseHandle(hToken);
            return result;
        }

        int shutdown(char op[256],int second){
            int uFlag;
            if (strcmp(op,"logoff")==0){
                uFlag = EWX_LOGOFF;
            }
            else if(strcmp(op,"shutdown")==0){
                uFlag = EWX_SHUTDOWN;
            }
            else if(strcmp(op,"reboot")==0){
                uFlag = EWX_REBOOT;
            }
            else{
                return -1;
            }
            sleep(second);

            // ログオフ/シャットダウン/再起動を実行する
            if (SetProcessPrivilege(GetCurrentProcess(), SE_SHUTDOWN_NAME, TRUE)) {
                ExitWindowsEx(uFlag, 0);
            }
            return 0;
        }

        /**
         * @fn
         * メッセージボックスを作成
         * @param tit タイトル
         * @param tex テキスト
         * @param button 0.[OK] 1.[OK][キャンセル] 2.[中止][再試行][無視] 3.[はい][いいえ][キャンセル] 4.[はい][いいえ] 5.[再試行][キャンセル]
         * @param icon 0.なし 1.注意 2.情報 3.質問 4.警告
         * @param help 0.なし 1.あり
         * @return int
         */
        int MsgBox(char tit[256],char tex[256],int button ,int icon,int help){
            int ans, forms;

            switch(button){
                case 1:
                    forms  = MB_OKCANCEL;
                    break;
                case 2:
                    forms = MB_ABORTRETRYIGNORE;        
                    break;
                case 3:
                    forms = MB_YESNOCANCEL;
                    break;
                case 4:
                    forms = MB_YESNO;
                    break;
                case 5:
                    forms = MB_RETRYCANCEL;
                    break;
                default:
                    forms = MB_OK;
                    break;

            }
            switch(icon){
                case 1:
                case MB_ICONWARNING:
                    forms |= MB_ICONWARNING;
                    break;
                case 2:
                case MB_ICONINFORMATION:
                    forms |= MB_ICONINFORMATION;
                    break;
                case 3:
                case MB_ICONQUESTION:
                    forms |= MB_ICONQUESTION;
                    break;
                case 4:
                case MB_ICONERROR:
                    forms |= MB_ICONERROR;
                    break;
                default:
                    break;

            }
            switch(help){
                case 2:
                case MB_HELP:
                    forms |= MB_HELP;
                    break;
                default:
                    break;

            }
            ans = MessageBox(NULL , tex , tit , forms); 
            return ans;
        }

    #endif
#endif