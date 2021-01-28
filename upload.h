#ifndef UPLOAD_H
#define UPLOAD_H

#include "cgicc/Cgicc.h"
#include "db_util.h"

struct Page_upload_rx;
struct Page_upload;

void upload(std::ostream&,cgicc::Cgicc&,DB,Page_upload_rx const&);
void show(std::ostream&,DB,Page_upload const&);

#endif
