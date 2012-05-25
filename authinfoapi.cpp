/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "authinfoapi.h"
#include "utils.h"

AuthInfoAPI::AuthInfoAPI(LinphoneAuthInfo *authInfo) :
		JSAPIAuto(APIDescription(this)), mAuthInfo(authInfo), mUsed(true) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", this);
	init_proxy();

}

AuthInfoAPI::AuthInfoAPI(const std::string &username, const std::string &userid,
		const std::string &passwd, const std::string &ha1, const std::string &realm) :
		JSAPIAuto(APIDescription(this)), mUsed(false) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", this);
	mAuthInfo = linphone_auth_info_new(username.c_str(), userid.c_str(), passwd.c_str(), ha1.c_str(), realm.c_str());
	init_proxy();
}

void AuthInfoAPI::init_proxy() {
	registerProperty("userid", make_property(this, &AuthInfoAPI::getUserid, &AuthInfoAPI::setUserid));
	registerProperty("username", make_property(this, &AuthInfoAPI::getUsername, &AuthInfoAPI::setUsername));
	registerProperty("passwd", make_property(this, &AuthInfoAPI::getPasswd, &AuthInfoAPI::setPasswd));
}

AuthInfoAPI::~AuthInfoAPI() {
	FBLOG_DEBUG("AuthInfoAPI::~AuthInfoAPI", this);
	if (!mUsed) {
		linphone_auth_info_destroy(mAuthInfo);
	}
}

std::string AuthInfoAPI::getUserid() const {
	return linphone_auth_info_get_userid(mAuthInfo);
}

void AuthInfoAPI::setUserid(const std::string &userid) {
	linphone_auth_info_set_userid(mAuthInfo, userid.c_str());
}

std::string AuthInfoAPI::getUsername() const {
	return linphone_auth_info_get_username(mAuthInfo);
}

void AuthInfoAPI::setUsername(const std::string &username) {
	linphone_auth_info_set_username(mAuthInfo, username.c_str());
}

std::string AuthInfoAPI::getPasswd() const {
	return linphone_auth_info_get_passwd(mAuthInfo);
}

void AuthInfoAPI::setPasswd(const std::string &passwd) {
	linphone_auth_info_set_passwd(mAuthInfo, passwd.c_str());
}

AuthInfoAPIPtr AuthInfoAPI::get(LinphoneAuthInfo *authInfo) {
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
	return shared_ptr;
}