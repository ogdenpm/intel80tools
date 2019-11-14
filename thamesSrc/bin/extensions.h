#pragma once

void addFileRef(char *fname, int flags);
void deleteFileRef(char *fname);
void genDependencies(char *depfile);
void mapTmpFile(char *fname);
