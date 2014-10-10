/**
 *
 * Sabre - A sabermetrically designed database for baseball statistics
 * Copyright (C) 2014  Stephen Schweizer (code@theindexzero.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "bb_defs.h"

#include <ctype.h>
#include <map>
#include <set>

namespace Baseball {

    template<typename R> class CoreTable;

    template<typename E>
    class Filter
    {
    public:
        typedef std::set<E> ValueSet;

        enum Operation {
            And,
            Or
        };

    public:

        Filter() {}
        Filter(const E& enumValue);
        Filter(const ValueSet& enumList);

        ~Filter() {}

        // returns true if added, false otherwise
        bool add(const E& enumValue);

        // returns true if removed, false otherwise
        bool remove(const E& enumValue);

        // returns true if the given enumValue is a member
        // of the local enum list
        bool test(const E& enumValue, const Operation& op) const;

    protected:

        ValueSet m_set;
    };

    template<typename E>
    class Value
    {
    public:
        E value;

        Filter<E> filter;
    };


    class CoreRecord
    {
    public:
        CoreRecord(const tag& t);

        virtual std::string print() const;

        // the approximate size on disk of this record
        virtual unsigned long weight() const;

        tag id() const { return m_tag; }

    protected:

        // tag identifier
        tag m_tag;
    };

    struct RecordYear
    {
        RecordYear() : m_null(true) {}

        bool isNull() const { return m_null; }
        void validate() { m_null = false; }

    protected:
        bool m_null;
    };


    template<typename R>
    class CoreReference;


    template<typename R>
    class CoreTable : public Baseball::Singleton<CoreTable<R>>
    {
        friend class CoreReference<R>;
    public:
        CoreTable() {}

        ~CoreTable() {
            Table::iterator it = m_table.begin();

            while (it != m_table.end()) {
                delete it->second;
                it++;
            }

            m_table.clear();
        }

        typedef std::list<R*> RecordList;

        static R* get(const std::string& ref)
        {
            // create tag reference;
            tag_ref t;
            tag_cpy_s(t.s, ref.c_str(), ref.length());

            // lookup tag
            Table::iterator it = getInstance()->m_table.find(t);

            if (it != getInstance()->m_table.end()) {
                return it->second;
            }

            return static_cast<R*>(0);
        }

        static R* get(const tag& ref)
        {
            tag_ref t;
            tag_cpy(t.s, ref.ref);

            // lookup tag
            Table::iterator it = getInstance()->m_table.find(t);

            if (it != getInstance()->m_table.end()) {
                return it->second;
            }

            return static_cast<R*>(0);
        }

        static void store(const tag& ref, R* val)
        {
            tag_ref t;
            tag_cpy(t.s, ref.ref);

            getInstance()->m_table[t] = val;
        }

        // Creates a new record in the database, returing a pointer to
        // that record.  If tag already refers to an entry, this function
        // behaves identical to get.
        static R* createRecord(const tag& ref)
        {
            R* r = static_cast<R*>(0);
            tag_ref t;

            tag_cpy(t.s, ref.ref);

            Table::iterator it = getInstance()->m_table.find(t);

            if (it == getInstance()->m_table.end()) {
                r = new R(ref);

                getInstance()->m_table[t] = r;
            } else {
                r = it->second;
            }

            return r;
        }

        static size_t count()
        {
            return getInstance()->m_table.size();
        }

        typedef bool (*filterFunc)(const R*);

        static RecordList filter(filterFunc func = NULL)
        {
            RecordList list;
            Table::const_iterator it = getInstance()->m_table.begin();

            if (func) {
                for (; it != getInstance()->m_table.end(); it++) {
                    if (func(it->second)) {
                        list.push_back(it->second);
                    }
                }
            } else {
                for (; it != getInstance()->m_table.end(); it++) {
                    list.push_back(it->second);
                }
            }

            return list;
        }

        typedef CoreReference<R> Reference;

        static Reference begin()
        {
            Reference r;
            r.m_it = getInstance()->m_table.begin();

            return r;
        }

        static Reference end()
        {
            Reference r;
            r.m_it = getInstance()->m_table.end();

            return r;
        }

    protected:

        struct tag_ref {
            char s[TAGLEN];
            bool operator<(const tag_ref& rhs) const
            {
                int i = 0;
                while ((i < (TAGLEN - 1)) &&
                       (tolower(s[i]) == tolower(rhs.s[i])))
                {
                    i++;
                }
                return (tolower(s[i]) < tolower(rhs.s[i]));
            }
        };

        typedef std::map<tag_ref, R*> Table;

        Table m_table;
    };

    template<typename R>
    class CoreReference
    {
        friend class CoreTable<R>;

    public:

        R* record() {
            return m_it->second;
        }

        void next() { m_it++; }
        void prev() { m_it--; }

        CoreReference<R>& operator=(const CoreReference<R>& rhs) {
            if (this != &rhs) {
                m_it = rhs.m_it;
            }

            return *this;
        }
        bool operator==(const CoreReference<R>& rhs) const {
            return (m_it == rhs.m_it);
        }
        bool operator!=(const CoreReference<R>& rhs) const {
            return (false == operator==(rhs));
        }

    private:
        typename CoreTable<R>::Table::iterator m_it;
    };


    enum TruncateFormat
    {
        TruncateNone    = 0,
        TruncateFill    = 1,
        TruncateEnquote = 2,
        TruncateElipsis = 4
    };

    // truncates the string to size len.  if elipsis is true, the string is
    // truncated to size len - 3 and '...' is added to the end.
    // if fill is true, then strings of length less than len will be filled with
    // whitespace.
    std::string truncate(const std::string& sz, size_t len,
                         unsigned int format = TruncateNone);
}
