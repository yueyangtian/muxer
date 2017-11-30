#include "muxer.h"
#include <string>
class AVmuxer
{
public:
	AVmuxer(const char* _format, 
		size_t _buf_out_size, 
		size_t _buf_in_size, 
		size_t _avbuf_out_size, 
		size_t _avbuf_in_size)
	{
		if(_format == NULL)
			format="nothing";
		else
			format = _format;
		buf_out.totalSize = _buf_out_size;
		buf_in.totalSize = _buf_in_size;
		avbuf_out_size = _avbuf_out_size;
		avbuf_in_size = _avbuf_in_size;

		ifmt = NULL;
		ofmt = NULL;
		ic = NULL;
		oc = NULL;
		
	}
	bool AVmuxerInit()
	{
		if(AVmuxerAllocBuf() == false)
			return false;
		
		if(AVmuxerInitMux() == false)
			return false;

		return true;
	}
	
	~AVmuxer()
	{
		AVmuxerDestory();
	}
	void AVmuxerDestory()
	{
		if(buf_out.ptr != NULL)
		{
			av_free(buf_out.ptr);
		}

		if(buf_in.ptr != NULL)
		{
			av_free(buf_in.ptr);
		}

		if(avbuf_out != NULL)
		{
			av_free(buf_in.ptr);
		}

		if(avbuf_in != NULL)
		{
			av_free(buf_out.ptr);
		}
	}
protected:
	static int input_iobuffer(void* buf_in, uint8_t *buf, int buflen)
	{
		
	}
protected:
	bool AVmuxerInitMux()
	{
		int ret = 0, i = 0;
		AVIOContext *avio = NULL;
		av_register_all();

		ic = avformat_alloc_context();
		if(ic == NULL)
		{
			return false;
		}

		avio = avio_alloc_context(avbuf_out, avbuf_out_size, 0, &buf_in, input_iobuffer, NULL, NULL);
		if(avio == NULL)
		{
			return false;
		}
		ic->pb = avio;
		ic->flags = AVFMT_FLAG_CUSTOM_IO;

		ret = avformat_open_input(&ic, "whaterver", NULL, NULL);
		if(ret < 0)
		{
			return false;
		}

		ret = avformat_find_stream_info(ic, NULL);
		if(ret < 0)
		{
			return false;
		}

		ofmt = av_guess_format(format.c_str(), NULL, NULL);
		if(ofmt == NULL)
		{
			return false;
		}

		ret = avformat_alloc_output_context2(&oc, ofmt, NULL, NULL);
		if(ret != 0)
		{
			return false;
		}

		avio = avio_alloc_context(avbuf_in, avbuf_in_size, 1, &buf_out, NULL, output_iobuffer, NULL);
		if(avio == NULL)
		{
			return false;
		}

		for(i = 0;i < ic->nb_streams; ++i)
		{
			AVStream *in_stream = ic->streams[i];
			AVStream *out_stream = avformat_new_stream(oc, in_stream->codec->codec);
			if(out_stream == NULL)
			{
				return false;
			}

			ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
			if(ret != 0)
			{
				return false;
			}

			out_stream->codec->codec_tag = 0;
			if(oc->oformat->flags & AVFMT_GLOBALHEADER)
			{
				out_stream->codec->flags|= AVFMT_GLOBALHEADER;
			}
		}
		
		return true;
	}
	bool AVmuxerAllocBuf()
	{
		buf_out.pos = 0;
		buf_out.realSize = 0;
		buf_out.ptr = (unsigned char*)av_malloc(buf_out.totalSize);
		if(buf_out.ptr == NULL)
		{
			return false;
		}

		buf_in.pos = 0;
		buf_in.realSize = 0;
		buf_in.ptr = (unsigned char*)av_malloc(buf_in.totalSize);
		if(buf_in.ptr == NULL)
		{
			return false;
		}

		avbuf_out = (unsigned char*)av_malloc(avbuf_out_size);
		if(avbuf_out == NULL)
		{
			return false;
		}

		avbuf_in = (unsigned char*)av_malloc(avbuf_in_size);
		if(avbuf_in == NULL)
		{
			return false;
		}
		
		return true;
	}
protected:
	AVIOBufferContext buf_out;
	AVIOBufferContext buf_in;
	unsigned char* avbuf_out;
	unsigned char* avbuf_in;
	size_t avbuf_out_size;
	size_t avbuf_in_size;
	std::string format;

	AVInputFormat *ifmt;
	AVInputFormat *ofmt;
	AVFormatContext *ic;
	AVFormatContext *oc;
	
};
