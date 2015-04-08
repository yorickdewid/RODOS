
#include "filesystem.h"
#include "rodos.h"


//FileSystem fileSystem;
File file;
FileSystem fileSystem;

class FatmosFile: public Thread {
public:
    FatmosFile(): Thread("FATMOSFile", 20, 10000) { }

    void run() {
        fileSystem.mountDisk(0);


        int errCode = file.open("test.txt", file.O_READ);
        PRINTF("open file test.txt errorcode = %d\n", errCode);
        file.close();
        fileSystem.remove("test.txt");
        PRINTF("File test.txt removed\n");

        PRINTF("please ^C to terminate\n");

    }

} fatmosFile;


