#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#define NAME_POSIX_MQ	"/my_mp"
#define LEN_RBUF	512
mqd_t mq_fd;

typedef struct {
    long mtype;
    char content[4096];
} MSG;

int start_msq_sender(char *srcfile);
int start_msq_reciver();

int main(int argc, char **argv)
{
    char buff_err[128];
    struct mq_attr mq_attr; 
    memset(&mq_attr, 0x00, sizeof(struct mq_attr));
    mq_attr.mq_maxmsg = 256; 
    mq_attr.mq_msgsize=8016;

    if (argc < 2) { 
	printf("Usage :%s < sender file | reciver | unlink>\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    printf("POSIX message queue test =========>\n");

    if (argv[1][0] == 'u') { 
	printf("Removed MQ:%s\n", NAME_POSIX_MQ);
	mq_unlink(NAME_POSIX_MQ);
	exit(EXIT_SUCCESS);
    }

    if ((mq_fd = mq_open(NAME_POSIX_MQ, O_RDWR|O_CREAT|O_EXCL, 0660, &mq_attr)) > 0) {
	printf("Success full generate message queue\n");
    }
    else  { 
	strerror_r(errno, buff_err, sizeof(buff_err));
	printf("Failure on generating message queue [%d]reason-%s\n", errno, buff_err);
	exit(EXIT_FAILURE);
    }

    switch(argv[1][0]) { 
	case 's':
	    printf("+ Sender start transaction with the file(%s).\n", argv[2]);
	    start_msq_sender(argv[2]);
	    printf("+ Finished.... Press any key will exit\n");
	    getchar();
	    break;
	case 'r':
	    printf("+ Receiver waiting for message\n");
	    start_msq_receiver();
	    break;
	default:
	    printf("** Unknown option, use sender or receiver\n");
	    return 1;
    }
    mq_close(mq_fd);
    return 0;
}

int start_msq_sender(char *srcfile) 
{
    int fd;
    char *p_map;
    struct stat st;
    MSG msg;
    int tlen = 0;
    static int mtype;
    fd = open(srcfile, O_RDONLY);
    if (fd  < 0) { 
	printf("Don't open source file ==> errno[%d]\n", errno);
	exit(EXIT_FAILURE);
    }
    if(fstat(fd, st) < 0) { 
	printf("getting stat within fd , errno:%d\n", errno);
	close(fd);
	exit(1);
    }
    p_map = mmap(0,st.st_size , PROT_READ, MAP_PRIVATE, fd, 0);
    if (p_map == MAP_FAILED) { 
	printf("don't generating mmap() in system\n");
	close(fd);
	exit(EXIT_FAILURE);
    }
    close(fd); // incase of using mmap it is ok to close file descriptor 

    if (mq_send(mq_fd, p_map, st.st_size, 0) == -1) { 
	perror("Fail:mq_send()");
	exit(EXIT_FAILURE);
    }
    return 1;
}

int start_msq_receiver(long mtype) 
{
    int n_recv;
    struct mq_attr mq_attr;
    char *p_buf;
    mq_getattr(mq_fd, &mq_attr);
    if ((p_buf = (char *)malloc(mq_attr.mq_msgsize)) == NULL)
	return -1;
    while (1) { 
	if (n_recv = mq_receive(mq_fd, p_buf, mq_attr.mq_msgsize, NULL) == -1) { 
	    printf("FAILE:mq_receive()");
	    return -1;
	}
	printf("+recv(%d)\n", n_recv);
    }
    return 0;
}
