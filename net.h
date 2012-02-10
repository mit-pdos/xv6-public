#pragma once

void netclose(int sock);
int netread(int, char *, int);
int netwrite(int, char *, int);
long netsocket(int, int, int);
long netbind(int, void *, int);
long netlisten(int, int);
long netaccept(int, void *, void *);

