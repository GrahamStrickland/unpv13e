#include "unp.h"

void sctpstr_cli(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen) {
  struct sockaddr_in peeraddr;
  struct sctp_sndrcvinfo sri;
  char sendline[MAXLINE], recvline[MAXLINE];
  socklen_t len;
  int out_sz, rd_sz;
  int msg_flags;

  bzero(&sri, sizeof(sri));
  while (fgets(sendline, MAXLINE, fp) != NULL) {
    if (sendline[0] != '[') {
      printf("Error, line must be of the form '[streamnum]text'\n");
      continue;
    }
    sri.sinfo_stream = strtol(&sendline[1], NULL, 0);
    out_sz = strlen(sendline);
    Sctp_sendmsg(sock_fd, sendline, out_sz, to, tolen, 0, 0, sri.sinfo_stream,
                 0, 0);
    /* include mod_strcli1 */
    do {
      len = sizeof(peeraddr);
      rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline), (SA *)&peeraddr,
                           &len, &sri, &msg_flags);
      if (msg_flags & MSG_NOTIFICATION)
        check_notification(sock_fd, recvline, rd_sz);
    } while (msg_flags & MSG_NOTIFICATION);
    printf("From str:%d seq:%d (assoc:0x%x):", sri.sinfo_stream, sri.sinfo_ssn,
           (u_int)sri.sinfo_assoc_id);
    printf("%.*s", rd_sz, recvline);
    /* end mod_strcli1 */
  }
}
