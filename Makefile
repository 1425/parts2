CXXFLAGS += -Wall -Wextra -Werror -std=c++2a -Os
CXX = g++-10
LDFLAGS=`mariadb_config --cflags --libs` -lmariadbclient -lcgicc
INSTALL_DIR ?= /home/eric/code/cgi-bin/

DEPS:=\
	migrate.o parting.o parting2d.o \
	stock.o parting3d.o db_util.o \
	insert.o get_name.o vars.o \
	orders.o search.o input.o \
	main.o extra.o nested.o \
	db.o data_types.o location.o \
	util.o recent.o latest.o \
	page.o item.o status.o \
	upload.o base64.o view.o \
	export.o type.o

db: db.h $(DEPS)
	$(CXX) $(DEPS) $(CXXFLAGS) $(LDFLAGS) -o db

all: run install

show: run install
	firefox http://localhost/cgi-bin/db?page=status

%.o: %.cpp *.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

parting: parting.cpp parting.h util.cpp util.h
	$(CXX) $(CXXFLAGS) parting.cpp util.cpp -o parting

parting2d: parting2d.cpp parting2d.h util.cpp util.h
	$(CXX) $(CXXFLAGS) parting2d.cpp util.cpp -o parting2d

parting3d: parting3d.cpp parting3d.h util.cpp data_types.h data_types.cpp util.h
	$(CXX) $(CXXFLAGS) parting3d.cpp util.cpp data_types.cpp -o parting3d

install: db
	cp db $(INSTALL_DIR)

run: db
	./db

clean:
	rm -f $(DEPS) db
