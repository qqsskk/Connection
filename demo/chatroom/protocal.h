/*************************************************************************
	> File Name: protocal.h
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月21日 星期日 15时21分14秒
 ************************************************************************/
enum ReqNo{
	REQ_LOGIN=1,
	REQ_REGISTER,
	REQ_SHOW,
	REQ_CHAT
};
struct ReqHead{
	int reqNo;
	int reqLength;
};
struct ReqLogin{
	struct ReqHead head;
	int nameLength;
	char str[0];/*actual size: head.reqLength-sizeof(head), username & passwd*/
};
struct ReqRegister{
	struct ReqHead head;
	int nameLength;
	char str[0];
};
struct ReqShow{
	struct ReqHead head;
};
struct ReqChat{
	struct ReqHead head;
	int nameLength;
	char str[0];
};
//-----------------------------------------------------------------------------
enum ReplyNo{
	REPLY_LOGIN=1,
	REPLY_REGISTER,
	REPLY_SHOW,
	REPLY_CHAT
};
struct ReplyHead{
	int replyNo;
	int replyLength;
};
struct ReplyLogin{
	struct ReplyHead head;
	bool ok;
};
struct ReplyRegister{
	struct ReplyHead head;
	bool ok;
};
struct UserInfo{
	int length;
	char name[0];
};
struct ReplyShow{
	struct ReplyHead head;
	int n;
	struct UserInfo user[0];
};
struct ReplyChat{
	struct ReplyHead head;
	int nameLength;
	char str[0];/*peer name and chat info*/
};
