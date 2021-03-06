/*
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This file is part of csdiff.
 *
 * csdiff is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * csdiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with csdiff.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H_GUARD_JSON_PARSER_H
#define H_GUARD_JSON_PARSER_H

#include "abstract-parser.hh"

class JsonParser: public AbstractParser {
    public:
        JsonParser(
                std::istream        &input,
                const std::string   &fileName,
                const bool           silent);

        virtual ~JsonParser();
        virtual bool getNext(Defect *);
        virtual bool hasError() const;
        virtual const TScanProps& getScanProps() const;

    private:
        JsonParser(const Parser &);
        JsonParser& operator=(const Parser &);

        struct Private;
        Private *d;
};

#endif /* H_GUARD_JSON_PARSER_H */
