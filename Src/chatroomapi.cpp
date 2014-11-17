/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2014 Belledonne Communications

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


 Authors:
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#include "chatroomapi.h"
#include "factoryapi.h"

namespace LinphoneWeb {

ChatRoomAPI::ChatRoomAPI(LinphoneChatRoom *chatRoom) :
		WrapperAPI(APIDescription(this)), mChatRoom(chatRoom) {
	FBLOG_DEBUG("ChatRoomAPI::ChatRoomAPI", "this=" << this << "\t" << "chatRoom=" << chatRoom);
	linphone_chat_room_ref(mChatRoom);
	linphone_chat_room_set_user_data(mChatRoom, this);
}

ChatRoomAPI::~ChatRoomAPI() {
	FBLOG_DEBUG("ChatRoomAPI::~ChatRoomAPI", "this=" << this);
	if (mChatRoom != NULL) {
		linphone_chat_room_set_user_data(mChatRoom, NULL);
		linphone_chat_room_unref(mChatRoom);
	}
}

void ChatRoomAPI::initProxy() {
	registerProperty("core", make_property(this, &ChatRoomAPI::getCore));
	registerProperty("historySize", make_property(this, &ChatRoomAPI::getHistorySize));
	registerProperty("peerAddress", make_property(this, &ChatRoomAPI::getPeerAddress));
	registerProperty("remoteComposing", make_property(this, &ChatRoomAPI::remoteComposing));

	registerMethod("compose", make_method(this, &ChatRoomAPI::compose));
	registerMethod("getHistoryRange", make_method(this, &ChatRoomAPI::getHistoryRange));
	registerMethod("newMessage", make_method(this, &ChatRoomAPI::createMessage));
	registerMethod("newMessage2", make_method(this, &ChatRoomAPI::createMessage2));
	registerMethod("sendMessage", make_method(this, &ChatRoomAPI::sendMessage));
}

void ChatRoomAPI::compose() {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::compose", "this=" << this);
	linphone_chat_room_compose(mChatRoom);
}

CoreAPIPtr ChatRoomAPI::getCore() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getCore", "this=" << this);
	return getFactory()->getCore(linphone_chat_room_get_core(mChatRoom));
}

ChatMessageAPIPtr ChatRoomAPI::createMessage(StringPtr const &message) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::createMessage", "this=" << this << "\t" << "message=" << message);
	LinphoneChatMessage *chatMessage = linphone_chat_room_create_message(mChatRoom, STRING_TO_CHARPTR(message));
	return getFactory()->getChatMessage(chatMessage);
}

ChatMessageAPIPtr ChatRoomAPI::createMessage2(StringPtr const &message, StringPtr const &externalBodyUrl, int state, time_t time, bool isRead, bool isIncoming) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::createMessage2", "this=" << this << "\t" << "message=" << message << "\t" << "externalBodyUrl=" << externalBodyUrl << "\t" << "time=" << time << "\t" << "isIncoming=" << isIncoming);
	LinphoneChatMessage *chatMessage = linphone_chat_room_create_message_2(mChatRoom, STRING_TO_CHARPTR(message), STRING_TO_CHARPTR(externalBodyUrl), (LinphoneChatMessageState)state, time, isRead, isIncoming);
	return getFactory()->getChatMessage(chatMessage);
}

std::vector<ChatMessageAPIPtr> ChatRoomAPI::getHistoryRange(int begin, int end) const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getHistoryRange", "this=" << this << "\t" << "begin=" << begin << "\t" << "end=" << end);
	std::vector<ChatMessageAPIPtr> list;
	for (const MSList *node = linphone_chat_room_get_history_range(mChatRoom, begin, end); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getChatMessage(reinterpret_cast<LinphoneChatMessage *>(node->data)));
	}
	return list;
}

int ChatRoomAPI::getHistorySize() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getHistorySize", "this=" << this);
	return linphone_chat_room_get_history_size(mChatRoom);
}

AddressAPIPtr ChatRoomAPI::getPeerAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getPeerAddress", "this=" << this);
	const LinphoneAddress *peerAddress = linphone_chat_room_get_peer_address(mChatRoom);
	return getFactory()->getAddress(peerAddress);
}

bool ChatRoomAPI::remoteComposing() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::remoteComposing", "this=" << this);
	return (linphone_chat_room_is_remote_composing(mChatRoom) == TRUE);
}

void ChatRoomAPI::sendMessage(ChatMessageAPIPtr chatMessage) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::sendMessage", "this=" << this << "\t" << "chatMessage=" << chatMessage);
	linphone_chat_room_send_message2(mChatRoom, chatMessage->getRef(), ChatRoomAPI::wrapper_message_state_changed, mChatRoom);
}



void ChatRoomAPI::onMessageStateChanged(LinphoneChatMessage *msg, LinphoneChatMessageState state) {
	FBLOG_DEBUG("ChatRoomAPI::onMessageStateChanged",  "this=" << this << "\t" << "msg=" << msg << "\t" << "state=" << state);
	fire_messageStateChanged(boost::static_pointer_cast<ChatRoomAPI>(shared_from_this()), getFactory()->getChatMessage(msg), state);
}

void ChatRoomAPI::wrapper_message_state_changed(LinphoneChatMessage *msg, LinphoneChatMessageState state, void *ud) {
	LinphoneChatRoom *room = (LinphoneChatRoom *)ud;
	if (linphone_chat_room_get_user_data(room) != NULL) {
		((ChatRoomAPI *)linphone_chat_room_get_user_data(room))->onMessageStateChanged(msg, state);
	} else {
		FBLOG_ERROR("ChatRoomAPI::wrapper_message_state_changed", "No proxy defined!");
	}
}

} // LinphoneWeb
