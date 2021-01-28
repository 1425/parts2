#ifndef MAIN_H
#define MAIN_H

#include<iosfwd>
#include<functional>

void page(std::ostream&,std::string const&,std::string const&);
void page(std::ostream&,std::string const& title,std::string const& heading,std::string const& body);

void page(std::ostream&,std::string const& title,std::string const& heading,std::string const& body,std::string const& search);

void page(
	std::ostream&,
	std::string const& title,
	std::string const& heading,
	std::function<void(std::ostream&)> const& body,
	std::string const& search
);

std::string tutorial();

#endif
