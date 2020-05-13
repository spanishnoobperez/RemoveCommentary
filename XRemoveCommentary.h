#ifndef _XREMOVECOMMENTARY_
#define _XREMOVECOMMENTARY_

class XRemoveCommentary {
private:
    int m_commentaryFlag;		//0:No comments 1:Single line comment 2:Multi-line comments
    FILE *m_pFile;				//File pointer
public:
    int removeCommentary(const char *fileName);
    int removeMultiEnter(const char *fileName);
    int removeBlankChar(const char *fileName);
};

#endif