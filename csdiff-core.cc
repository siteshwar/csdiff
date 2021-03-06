/*
 * Copyright (C) 2011-2012 Red Hat, Inc.
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

#include "csdiff-core.hh"

#include "cswriter.hh"
#include "deflookup.hh"
#include "json-writer.hh"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

// FIXME: some keys should be merge more intelligently if they already exist
// TODO: define a nesting limit for keys like diffbase-diffbase-diffbase-...
void mergeScanProps(TScanProps &props, const TScanProps &oldProps)
{
    BOOST_FOREACH(TScanProps::const_reference item, oldProps) {
        const std::string &oldKey = item.first;
        const std::string &oldVal = item.second;
        std::string key("diffbase-");
        key += oldKey;
        props[key] = oldVal;
    }
}

bool /* anyError */ diffScans(
        std::ostream               &strDst,
        std::istream               &strOld,
        std::istream               &strNew,
        const std::string          &fnOld,
        const std::string          &fnNew,
        const bool                  showInternal,
        const bool                  silent,
        EFileFormat                 format,
        const EColorMode            cm)
{
    // create Parsers
    Parser pOld(strOld, fnOld, silent);
    Parser pNew(strNew, fnNew, silent);

    // decide which format use for the output
    if (format == FF_AUTO)
        format = pNew.inputFormat();

    // create the appropriate writer
    boost::shared_ptr<AbstractWriter> writer;
    if (format == FF_JSON)
        writer.reset(new JsonWriter(strDst));
    else
        writer.reset(new CovWriter(strDst, cm));

    // propagate scan properties if available
    TScanProps props = pNew.getScanProps();
    mergeScanProps(props, pOld.getScanProps());
    writer->setScanProps(props);

    // read old
    DefLookup stor(/* TODO: document this side effect */ showInternal);
    Defect def;
    while (pOld.getNext(&def))
        stor.hashDefect(def);

    // read new
    while (pNew.getNext(&def)) {
        if (stor.lookup(def))
            continue;

        if (!showInternal) {
            const DefEvent &keyEvt = def.events[def.keyEventIdx];
            if (keyEvt.event == "internal warning")
                // we suppress internal warnings by default
                continue;
        }

        // a newly added defect found
        writer->handleDef(def);
    }

    writer->flush();

    return pOld.hasError()
        || pNew.hasError();
}
