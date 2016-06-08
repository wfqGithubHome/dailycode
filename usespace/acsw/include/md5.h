class CMD5  
{
private:
	unsigned long int state[4];
	unsigned long int count[2];
	unsigned char buffer[64];
	unsigned char PADDING[64];
	void MD5Init();
	void MD5_memset(unsigned char* output,int value,unsigned int len);
	void MD5_memcpy(unsigned char* output,unsigned char* input,unsigned int len);
	void MD5Transform(unsigned long int state[4],unsigned char block[64]);
	void Decode(unsigned long int *output,unsigned char *input,unsigned int len);
	void Encode(unsigned char *output,unsigned long int *input,unsigned int len);
public:
	CMD5();
	virtual ~CMD5();
	void MD5Update(unsigned char *input,unsigned int inputLen);
	void MD5Final(unsigned char digest[16]);

};
