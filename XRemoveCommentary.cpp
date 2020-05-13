#include "pch.h"
#include "XRemoveCommentary.h"
#include <cstring>
#include <cstdio>
#include <windows.h>
#include <cstdlib>
#pragma warning(disable:4996)

#define MAX_FILE_NAME_LENGTH	256
#define MAX_BLANK_CHAR_SUM		1024

LPCTSTR c2t(const char *c) {
    WCHAR *wszClassName = new WCHAR[strlen(c) + 1];
    memset(wszClassName, 0, sizeof(wszClassName));
    MultiByteToWideChar(CP_ACP, 0, c, strlen(c) + 1, wszClassName,
                        sizeof(wszClassName) / sizeof(wszClassName[0]));
    return wszClassName;
}

int XRemoveCommentary::removeCommentary(const char *fileName) {
    if((m_pFile = fopen(fileName, "r")) == NULL) {
        return 0;
    }
    FILE  *pArmFile;
    char tempArmFileName[MAX_FILE_NAME_LENGTH];
    int i;
    for(i = 0; i < MAX_FILE_NAME_LENGTH; i ++) {
        tempArmFileName[i] = fileName[i];
        if(fileName[i] == '\0') {
            if(i >= MAX_FILE_NAME_LENGTH - 5) {
                fclose(m_pFile);
                return 0;
            } else {
                tempArmFileName[i] = '.';
                tempArmFileName[i + 1] = 't';
                tempArmFileName[i + 2] = 'o';
                tempArmFileName[i + 3] = 'p';
                tempArmFileName[i + 4] = '\0';
                break;
            }
        }
    }

    if(i >= MAX_FILE_NAME_LENGTH - 5) {
        fclose(m_pFile);
        return 0;
    }

    if((pArmFile = fopen(tempArmFileName, "w")) == NULL) {
        if((pArmFile = fopen(tempArmFileName, "a")) == NULL) {
            return 0;
        }
    }
    //scanning source file
    char temp_read;
    m_commentaryFlag = 0;
    int tempFlag = 0;		//flag for slash
    int doubleFlag = 0;		//flag for double quotes 0:Does not have 1:Exist one
    int singleFlag = 0;		//flag for single quote 0:Does not have 1:Exist one
    char upChar = ' ';		//The last string read

    while(true) {
        if(fread(&temp_read, 1, 1, m_pFile) == 0) break;
        if(m_commentaryFlag == 0) {
            //No head
            if(temp_read == '"' && upChar != '\\' && singleFlag == 0) {
                //Process for double quotes
                if(doubleFlag == 0) doubleFlag = 1;
                else doubleFlag = 0;
            }
            if(temp_read == '\'' && upChar != '\\' && doubleFlag == 0) {
                //Process for single quote
                if(singleFlag == 0) singleFlag = 1;
                else singleFlag = 0;
            }
            if(temp_read == '/' && doubleFlag == 0) {
                //If it is / you need to wait until next time to judge whether to output
                tempFlag ++;
                if(tempFlag == 2) {
                    //Single line commentary
                    m_commentaryFlag = 1;
                    tempFlag = 0;
                }
            } else if(temp_read == '*' && tempFlag == 1 && doubleFlag == 0) {
                //Multiple-line commentaries
                m_commentaryFlag = 2;
                tempFlag = 0;
            } else if(tempFlag == 1) {
                //If you encounter / but it is not a comment, then add the output here
                tempFlag = 0;
                char temp_head = '/';
                fwrite(&temp_head, 1, 1, pArmFile);
                fwrite(&temp_read, 1, 1, pArmFile);
            } else {
                fwrite(&temp_read, 1, 1, pArmFile);
            }
        } else {
            //Have head
            //Judge whether it finished
            if(m_commentaryFlag == 2) {
                if(temp_read == '*') tempFlag = 1;
                if(temp_read == '/' && tempFlag == 1) tempFlag = 2;	//Finish
                if(temp_read != '*' && tempFlag == 1) tempFlag = 0;
            }

            if(m_commentaryFlag == 1 && temp_read == '\n') {
                //Single-line comment line breaks require output
                m_commentaryFlag = 0;
                tempFlag = 0;
                fwrite(&temp_read, 1, 1, pArmFile);
            } else if(m_commentaryFlag == 2 && tempFlag == 2) {
                //Nothing will be output in the multi-line comments
                m_commentaryFlag = 0;
                tempFlag = 0;
            }

        }
        upChar = temp_read;
        if(feof(m_pFile) == 1) break;
    }
    //Close file
    fclose(m_pFile);
    fclose(pArmFile);
    //Remove whitespace characters at the end of the line
    removeBlankChar(tempArmFileName);
    //Delete extra files
    DeleteFile(c2t(tempArmFileName));
    return 1;
}

int XRemoveCommentary::removeMultiEnter(const char *fileName) {
    //create arm file
    FILE  *pArmFile;
    char tempArmFileName[MAX_FILE_NAME_LENGTH];
    int i;
    for(i = 0; i < MAX_FILE_NAME_LENGTH; i ++) {
        tempArmFileName[i] = fileName[i];
        if(fileName[i] == '\0') {
            if(i >= MAX_FILE_NAME_LENGTH - 5) {
                fclose(m_pFile);
                return 0;
            } else {
                tempArmFileName[i - 4] = '.';
                tempArmFileName[i - 3] = 't';
                tempArmFileName[i - 2] = 'm';
                tempArmFileName[i - 1] = 'p';
                tempArmFileName[i - 0] = '\0';
                break;
            }
        }
    }

    if(i >= MAX_FILE_NAME_LENGTH - 5) {
        fclose(m_pFile);
        return 0;
    }

    if((pArmFile = fopen(tempArmFileName, "w")) == NULL) {
        if((pArmFile = fopen(tempArmFileName, "a")) == NULL) {
            return 0;
        }
    }
    //open source file
    char fileName1[MAX_FILE_NAME_LENGTH];
    strcpy(fileName1, fileName);
    fileName1[strlen(fileName) - 4] = '\0';
    if((m_pFile = fopen(fileName1, "r")) == NULL) {
        return 0;
    }
    //exchange twe files
    unsigned char *pFileData = NULL;
    int fileLen;
    fseek(m_pFile, 0, SEEK_END);
    fileLen = ftell(m_pFile);	//Get the length of the file
    fseek(m_pFile, 0, SEEK_SET);
    try {
        pFileData = new unsigned char[fileLen];
        if(pFileData == NULL) return 0;
    } catch(...) {
        return 0;
    }
    fread(pFileData, fileLen, 1, m_pFile);
    fwrite(pFileData, fileLen, 1, pArmFile);
    fclose(m_pFile);
    fclose(pArmFile);
    delete[] pFileData;
    //Reopen the file
    if((m_pFile = fopen(fileName, "r")) == NULL) return 0;
    if((pArmFile = fopen(fileName1, "w")) == NULL) {
        if((pArmFile = fopen(fileName1, "a")) == NULL) return 0;
    }

    //scanning source file
    char temp_read;
    char upChar = '\n';			//The last string read (when entering for the first time, all the above reads are line breaks by default)
    char upUpChar = '\n';		//The last string read
    while(1) {
        if(fread(&temp_read, 1, 1, m_pFile) == 0) break;
        if(!(upChar == '\n' && upUpChar == '\n' && temp_read == '\n')) { //There's no conflicts logically
            upUpChar = upChar;
            upChar = temp_read;
            fwrite(&temp_read, 1, 1, pArmFile);	//Output the read result
        }
        if(feof(m_pFile) == 1) break;
    }
    //Close the file
    fclose(m_pFile);
    fclose(pArmFile);
    return 1;
}

int XRemoveCommentary::removeBlankChar(const char *fileName) {
    //open source file
    if((m_pFile = fopen(fileName, "r")) == NULL) {
        return 0;
    }
    //create arm file
    FILE  *pArmFile;
    char tempArmFileName[MAX_FILE_NAME_LENGTH];
    int i;
    for(i = 0; i < MAX_FILE_NAME_LENGTH; i ++) {
        tempArmFileName[i] = fileName[i];
        if(fileName[i] == '\0') {
            if(i >= MAX_FILE_NAME_LENGTH - 5) {
                fclose(m_pFile);
                return 0;
            } else {
                tempArmFileName[i - 4] = '.';
                tempArmFileName[i - 3] = 't';
                tempArmFileName[i - 2] = 'n';
                tempArmFileName[i - 1] = 'p';
                tempArmFileName[i - 0] = '\0';
                break;
            }
        }
    }

    if(i >= MAX_FILE_NAME_LENGTH - 5) {
        fclose(m_pFile);
        return 0;
    }

    if((pArmFile = fopen(tempArmFileName, "w")) == NULL) {
        if((pArmFile = fopen(tempArmFileName, "a")) == NULL) {
            return 0;
        }
    }
    //scanning source file
    char temp_read;
    char blankChar[MAX_BLANK_CHAR_SUM] = "";
    int blankCharSum = 0;

    while(1) {
        if(fread(&temp_read, 1, 1, m_pFile) == 0) break;
        if(temp_read == '\n') {
            //Clear blank records
            fwrite(&temp_read, 1, 1, pArmFile);
            blankCharSum = 0;
        } else if(temp_read == ' ' || temp_read == '\t') {
            blankChar[blankCharSum] = temp_read;
            blankCharSum++;
            if(blankCharSum >= MAX_BLANK_CHAR_SUM) {
                printf("Error!\n");
                exit(1);
            }
        } else {
            if(blankCharSum != 0) {
                //There is a fke ending blank
                fwrite(&blankChar, blankCharSum, 1, pArmFile);
            }
            blankCharSum = 0;
            fwrite(&temp_read, 1, 1, pArmFile);
        }

        if(feof(m_pFile) == 1) break;
    }
    //Close the file
    fclose(m_pFile);
    fclose(pArmFile);
    //Remove multi-line whitespace
    removeMultiEnter(tempArmFileName);
    //Remove extra files
    DeleteFile(c2t(tempArmFileName));
    char fileTemp[512] = "";
    return 1;
}