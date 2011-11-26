/*!
 * @file  realserver_element.h
 * @brief realserver data prototype
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#ifndef REALSERVER_ELEMENT_H
#define REALSERVER_ELEMENT_H
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/format.hpp>
#include "endpoint.h"

namespace l7vs
{

class realserver_element
{
protected:
        int nactive;
        int ninact;
public:
        enum REALSERVER_FWDMODE_TAG {
                FWD_NONE = 0,
                FWD_MASQ,
                FWD_TPROXY,
        };

        boost::asio::ip::tcp::endpoint tcp_endpoint;
        boost::asio::ip::udp::endpoint udp_endpoint;
        int weight;
        REALSERVER_FWDMODE_TAG fwdmode;

        // constructor
        realserver_element()
                :
                nactive(0),
                ninact(0),
                weight(-1),
                fwdmode(FWD_NONE)
        {}

        realserver_element(const realserver_element &in)
                :
                nactive(in.nactive),
                ninact(in.ninact),
                tcp_endpoint(in.tcp_endpoint),
                udp_endpoint(in.udp_endpoint),
                weight(in.weight),
                fwdmode(in.fwdmode)
        {}

        realserver_element &operator=(const realserver_element &elem) {
                nactive = elem.nactive;
                ninact = elem.ninact;
                tcp_endpoint = elem.tcp_endpoint;
                udp_endpoint = elem.udp_endpoint;
                weight = elem.weight;
                fwdmode = elem.fwdmode;
                return *this;
        }

        friend bool operator==(const realserver_element &rselem1, const realserver_element &rselem2) {
                return rselem1.tcp_endpoint == rselem2.tcp_endpoint &&
                       rselem1.udp_endpoint == rselem2.udp_endpoint &&
                       rselem1.weight == rselem2.weight &&
                       rselem1.fwdmode == rselem2.fwdmode;
        }

        friend bool operator!=(const realserver_element &rselem1, const realserver_element &rselem2) {
                return rselem1.tcp_endpoint != rselem2.tcp_endpoint ||
                       rselem1.udp_endpoint != rselem2.udp_endpoint ||
                       rselem1.weight != rselem2.weight ||
                       rselem1.fwdmode != rselem2.fwdmode;
        }

        friend bool operator<(const realserver_element &rselem1, const realserver_element &rselem2) {
                if (rselem1.tcp_endpoint < rselem2.tcp_endpoint) {
                        return true;
                } else if (rselem1.tcp_endpoint != rselem2.tcp_endpoint) {
                        return false;
                } else if (rselem1.weight < rselem2.weight) {
                        return true;
                } else if (rselem1.weight > rselem2.weight) {
                        return false;
                } else {
                        return rselem1.fwdmode < rselem2.fwdmode;
                }
        }

        int get_active() {
                return nactive;
        }

        int get_inact() {
                return ninact;
        }

        void set_active(const int in_active) {
                nactive = in_active;
        }

        void set_inact(const int in_inact) {
                ninact = in_inact;
        }

        const std::string get_fwdmode_str() {
                return fwdmode == FWD_MASQ   ? "Masq"
                       : fwdmode == FWD_TPROXY ? "Tproxy"
                       : "Unknown";
        }

        template <typename Elem, typename Traits>
        friend std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const realserver_element &elem) {
                os << "realserver_element={";
                os << boost::format("nactive=%d, "
                                    "ninact=%d, "
                                    "tcp_endpoint=%s, "
                                    "udp_endpoint=%s, "
                                    "weight=%d, "
                                    "fwdmode=%d}")
                   % elem.nactive
                   % elem.ninact
                   % elem.tcp_endpoint
                   % elem.udp_endpoint
                   % elem.weight
                   % elem.fwdmode;
                return os;
        }

private:
        friend class boost::serialization::access; //! friend boost serializable class
        //! serializable
        //! @brief using boost serializable. class serializable function.
        //! @param[in] archive
        //! @param[in] version
        template <class Archive>
        void serialize(Archive &ar, const unsigned int version) {
                ar &nactive;
                ar &ninact;
                ar &tcp_endpoint;
                ar &udp_endpoint;
                ar &weight;
                ar &fwdmode;
        }
};

} //namespace l7vsd
#endif //REALSERVER_ELEMENT
