#include <sys/syscall.h>
#include <sys/fcntl.h>

#define BUFFER_SIZ  (256)

static char buffer[256];

int main(int argc, char *argv[]){
    int i, ret;
    char *src, *dest;
    int src_fd, dest_fd;
    struct stat src_buf, dest_buf;
    if(argc < 3){
        fprintf(stderr, "cp [OPTION]... SOURCE DEST\n");
        return 1;
    }
    argv++;
    src = *argv++;
    dest = *argv;
    src_fd = open(src, O_RDONLY);
    if(src_fd < 0){
        perror(src);
        return 1;
    }
    dest_fd = open(dest, O_WRONLY | O_CREAT);
    if(dest_fd < 0){
        perror(dest);
        return 1;
    }
    ret = fstat(src_fd, &src_buf);
    if(ret)
        return ret;
    ret = fstat(dest_fd, &dest_buf);
    if(ret)
        return ret;
    if(!S_ISREG(src_buf.st_mode) || !S_ISREG(dest_buf.st_mode)){
        fprintf(stderr, "not regular file %s %s", src, dest);
        return 1;
    }

    while((ret = read(src_fd, buffer, BUFFER_SIZ)) > 0){
        ret = write(dest_fd, buffer, ret);
    }
    return 0;
}


