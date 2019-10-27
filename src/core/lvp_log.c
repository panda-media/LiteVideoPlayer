#include "lvp_log.h"

void lvp_print(struct lvp_log *log_ctx,int level,const char *file,const char *func,int line, const char* fmt, ...){
	char log[2048] = { 0 };
	char log_fmt[2500] = { 0 };
	int write_index = 5;

	log[0] = '[';
	log[1] = 'D';
	log[2] = ']';
	log[3] = ' ';
	log[4] = '[';

	time_t t;
	time(&t);

	struct tm *time_info = localtime(&t);

	//log:"[D]function"
	int func_len = (int)strlen(func);
	memcpy(log+write_index, func, func_len);
	write_index += func_len;

	//log:"[D]function:"
	log[write_index] = ':';
	write_index++;

    

	//log:"[D]function:10"
	char line_str[8] = { 0 };
	sprintf(line_str,"%d",line);
	line_str[7] = 0;
	int line_len = (int)strlen(line_str);
	memcpy(log + write_index, line_str, line_len);
	write_index += line_len;
	log[write_index++] = ']';
	log[write_index++] = ' ';

	//log:"[E/W/D]function:10"
	switch (level)
	{
	case LVP_LOG_DEBUG:
		log[1] = 'D';
		break;
	case LVP_LOG_WARIN:
		log[1] = 'W';
		break;
	case LVP_LOG_ERROR:
		log[1] = 'E';
		break;
	default:
		log[1] = 'I';
		break;
	}

	
    va_list args;
    va_start(args,fmt);
    vsprintf(log+write_index,fmt,args);
    va_end(args);

    const char *split = "*******************************************";
	if(log_ctx != NULL&&log_ctx->ctx_name){
		sprintf(log_fmt,"%s\n%s\n[LVP:%s][%d:%d:%d]%s",split,file,log_ctx->ctx_name,time_info->tm_hour, time_info->tm_min, time_info->tm_sec,log);
	}else{
		sprintf(log_fmt,"%s\n%s\n[LVP][%d:%d:%d]%s",split,file,time_info->tm_hour, time_info->tm_min, time_info->tm_sec,log);
	}
    printf("%s\n",log_fmt);
	if (log_ctx!=NULL&&log_ctx->log_call!= NULL) {
        log_ctx->log_call(log_fmt,log_ctx->usr_data);
	}
}