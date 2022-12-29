#include "js_folder_notifier.h"

#include "jsvm/jsvm.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include "common.h"
#include "logger.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif


namespace fs = std::filesystem;

struct FileInfo {
    std::string path;
    int hashtime;
};

static struct {
    std::vector<FileInfo> files;
    std::string dir;
    std::thread thread;
    int finished;
} notifier;

#ifdef _WIN32

int js_vm_notifier_watch_directory(const char *lpDir)
{
    DWORD dwWaitStatus;
    HANDLE dwChangeHandles[2];
    char lpDirStorage[MAX_PATH];
    strncpy(lpDirStorage, lpDir, MAX_PATH);

    // Watch the directory for file creation and deletion.
    dwChangeHandles[0] = FindFirstChangeNotification(
                             lpDir,                         // directory to watch
                             FALSE,                         // do not watch subtree
                             FILE_NOTIFY_CHANGE_SIZE); // watch file modify

    if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) {
        Logger::error("FindFirstChangeNotification function failed er={}", GetLastError());
        return 0;
    }

    // Watch the subtree for directory creation and deletion.
    dwChangeHandles[1] = FindFirstChangeNotification(
                             lpDirStorage,                         // directory to watch
                             FALSE,                          // watch the subtree
                             FILE_NOTIFY_CHANGE_LAST_WRITE);  // watch file size change

    if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) {
        Logger::error("FindFirstChangeNotification function failed er= {}", GetLastError());
        return 0;
    }


    // Make a final validation check on our handles.
    if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL)) {
        Logger::error("Unexpected NULL from FindFirstChangeNotification er= {}", GetLastError());
        return 0;
    }

    // Change notification is set. Now wait on both notification
    // handles and refresh accordingly.
    while (1) {
        // Wait for notification.

        Logger::error("Waiting for notification...");

        dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, FALSE, INFINITE);

        switch (dwWaitStatus) {
            case WAIT_OBJECT_0:

                // A file was created, renamed, or deleted in the directory.
                // Refresh this directory and restart the notification.

                if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE ) {
                    Logger::error("FindNextChangeNotification function failed er={}", GetLastError());
                    return 0;
                }
                return 2;
                break;

            case WAIT_OBJECT_0 + 1:

                // A directory was created, renamed, or deleted.
                // Refresh the tree and restart the notification.

                if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE ) {
                    Logger::error("FindNextChangeNotification function failed er={}", GetLastError());
                    return 0;
                }
                return 3;
                break;

            case WAIT_TIMEOUT:

                // A timeout occurred, this would happen if some value other
                // than INFINITE is used in the Wait call and no changes occur.
                // In a single-threaded environment you might not want an
                // INFINITE wait.

                Logger::error("No changes in the timeout period.");
                break;

            default:
                Logger::error("Unhandled dwWaitStatus.");
                return 0;
        }
    }

    return 1;
}

#elif defined(__linux__) || defined(__android__)
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
int js_vm_notifier_watch_directory(const char *lpDir)
{
    int length, i = 0;
    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if ( fd < 0 ) {
        Logger::error( "WARNING !!! Cant init inotify for {}", lpDir);
        return 0;
    }

    wd = inotify_add_watch( fd, lpDir, IN_MODIFY );
    length = read( fd, buffer, BUF_LEN );

    if ( length < 0 ) {
        Logger::error( "WARNING !!! Cant read inotify event {}", lpDir);
        return 0;
    }

    int result = 1;
    while ( i < length ) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len ) {
            if ( event->mask & IN_MODIFY ) {
                if ( event->mask & IN_ISDIR ) {
                    Logger::error( "WARNING !!! The directory was modified {}", event->name);
                } else {
                    Logger::error( "WARNING !!! The file was modified {}", event->name);
                    result = 2;
                    break;
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }

    inotify_rm_watch( fd, wd );
    close( fd );

    return result;
}
#else
int js_vm_notifier_watch_directory(const char *lpDir)
{
    return 1;
}
#endif

static int get_time_modified(const char *path, struct tm *ftime)
{
    struct tm *footime;
#ifndef _WIN32
    struct stat attrib;
    if (stat(path, &attrib) != 0)
        return 1;
#else
    struct _stat64i32 attrib;
    if (_stat(path, &attrib) != 0) {
        return 1;
    }
#endif
    footime = gmtime((const time_t *) & (attrib.st_mtime));
    memcpy(ftime, footime, sizeof(time_t));

    return 0;
}

void js_vm_notifier_create_snapshot(const std::string &folder)
{
    
    struct tm ftime;
    notifier.files.clear();

    std::vector<std::string> js_files;
    fs::path path = fs::current_path()/"scripts";
    if (!fs::exists(path)) {
        Logger::error("Scripts directory not found");
        return;
    }

    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".js")
            js_files.push_back(entry.path().string());
    }

    for (int i = 0; i < js_files.size(); ++i) {
        const char *js_path = js_files[i].c_str();

        get_time_modified(js_path, &ftime);

        FileInfo fi;
        fi.hashtime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        fi.path = js_path;

        notifier.files.push_back(fi);
    }
}

void js_vm_notifier_check_snapshot(void)
{
    struct tm ftime;

    for (int i = 0; i < notifier.files.size(); ++i) {
        const std::string &js_path = notifier.files[i].path;
        if (js_path.empty()) {
            continue;
        }

        get_time_modified(js_path.c_str(), &ftime);

        unsigned int newTime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        unsigned int oldTime = notifier.files[i].hashtime;
        if( newTime != oldTime ) {
            notifier.files[i].hashtime = newTime;
            js_vm_reload_file(js_path.c_str());
        }
    }
}

static int js_vm_notifier_watch_directory_thread()
{
    int result;
    while (!notifier.finished) {
        result = js_vm_notifier_watch_directory( notifier.dir.c_str() );
        switch( result ) {
            case 0:
                notifier.finished = 1;
                break;

            case 3:
            case 2:
                js_vm_notifier_check_snapshot();
                notifier.finished = 0;
                break;

            default :
                notifier.finished = 0;
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}

void js_vm_notifier_watch_directory_init(const std::string &dir)
{
    Logger::info("start wtaching dir {}", dir);

    notifier.dir = dir;
    js_vm_notifier_create_snapshot(dir);

    notifier.thread = std::thread(js_vm_notifier_watch_directory_thread);
}
