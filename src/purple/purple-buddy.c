/**
 * @file purple-buddy.c
 *
 * pidgin-sipe
 *
 * Copyright (C) 2010-11 SIPE Project <http://sipe.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

#include "purple-private.h"

#include "blist.h"
#include "notify.h"
#include "privacy.h"
#include "version.h"

#include "sipe-common.h"
#include "sipe-backend.h"
#include "sipe-core.h"
#include "sipe-nls.h"

static GHashTable *info_to_property_table = NULL;

/** Property names to store in blist.xml */
#define ALIAS_PROP			"alias"
#define EMAIL_PROP			"email"
#define PHONE_PROP			"phone"
#define PHONE_DISPLAY_PROP		"phone-display"
#define PHONE_MOBILE_PROP		"phone-mobile"
#define PHONE_MOBILE_DISPLAY_PROP	"phone-mobile-display"
#define PHONE_HOME_PROP			"phone-home"
#define PHONE_HOME_DISPLAY_PROP		"phone-home-display"
#define PHONE_OTHER_PROP		"phone-other"
#define PHONE_OTHER_DISPLAY_PROP	"phone-other-display"
#define PHONE_CUSTOM1_PROP		"phone-custom1"
#define PHONE_CUSTOM1_DISPLAY_PROP	"phone-custom1-display"
#define SITE_PROP			"site"
#define COMPANY_PROP			"company"
#define DEPARTMENT_PROP			"department"
#define TITLE_PROP			"title"
#define OFFICE_PROP			"office"
/** implies work address */
#define ADDRESS_STREET_PROP		"address-street"
#define ADDRESS_CITY_PROP		"address-city"
#define ADDRESS_STATE_PROP		"address-state"
#define ADDRESS_ZIPCODE_PROP		"address-zipcode"
#define ADDRESS_COUNTRYCODE_PROP	"address-country-code"

#define ADD_PROP(key,value) g_hash_table_insert(info_to_property_table, (gpointer)key, value)

static void
init_property_hash(void)
{
	info_to_property_table = g_hash_table_new(NULL, NULL);

	ADD_PROP(SIPE_BUDDY_INFO_DISPLAY_NAME         , ALIAS_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_EMAIL                , EMAIL_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_WORK_PHONE           , PHONE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_WORK_PHONE_DISPLAY   , PHONE_DISPLAY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_SITE                 , SITE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_COMPANY              , COMPANY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_DEPARTMENT           , DEPARTMENT_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_JOB_TITLE            , TITLE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_OFFICE               , OFFICE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_STREET               , ADDRESS_STREET_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_CITY                 , ADDRESS_CITY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_STATE                , ADDRESS_STATE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_ZIPCODE              , ADDRESS_ZIPCODE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_COUNTRY              , ADDRESS_COUNTRYCODE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_MOBILE_PHONE         , PHONE_MOBILE_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_MOBILE_PHONE_DISPLAY , PHONE_MOBILE_DISPLAY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_HOME_PHONE           , PHONE_HOME_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_HOME_PHONE_DISPLAY   , PHONE_HOME_DISPLAY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_OTHER_PHONE          , PHONE_OTHER_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_OTHER_PHONE_DISPLAY  , PHONE_OTHER_DISPLAY_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_CUSTOM1_PHONE        , PHONE_CUSTOM1_PROP);
	ADD_PROP(SIPE_BUDDY_INFO_CUSTOM1_PHONE_DISPLAY, PHONE_CUSTOM1_DISPLAY_PROP);
}

static gchar *
sipe_buddy_info_to_purple_property(sipe_buddy_info_fields info)
{
	if (!info_to_property_table)
		init_property_hash();
	return g_hash_table_lookup(info_to_property_table, (gpointer)info);
}

sipe_backend_buddy sipe_backend_buddy_find(struct sipe_core_public *sipe_public,
					   const gchar *buddy_name,
					   const gchar *group_name)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;
	PurpleGroup *purple_group;

	if (group_name)
	{
		purple_group = purple_find_group(group_name);
		if (!purple_group)
			return NULL;

		return purple_find_buddy_in_group(purple_private->account,
						  buddy_name,
						  purple_group);
	} else {
		return purple_find_buddy(purple_private->account,
					 buddy_name);
	}
}

GSList* sipe_backend_buddy_find_all(struct sipe_core_public *sipe_public,
				    const gchar *buddy_name,
				    const gchar *group_name)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	if (group_name)
	{
		SIPE_DEBUG_ERROR_NOFORMAT("Finding all buddies in a group not supported on purple");
		return NULL;
	}

	return purple_find_buddies(purple_private->account, buddy_name);
}

gchar* sipe_backend_buddy_get_name(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				   const sipe_backend_buddy who)
{
	return g_strdup(((PurpleBuddy*)who)->name);
}

gchar* sipe_backend_buddy_get_alias(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				    const sipe_backend_buddy who)
{
	return g_strdup(purple_buddy_get_alias(who));
}

gchar* sipe_backend_buddy_get_server_alias(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
					   const sipe_backend_buddy who)
{
	return g_strdup(purple_buddy_get_server_alias(who));
}

gchar *sipe_backend_buddy_get_local_alias(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
					  const sipe_backend_buddy who)
{
	return g_strdup(purple_buddy_get_local_alias(who));
}

gchar* sipe_backend_buddy_get_group_name(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
					 const sipe_backend_buddy who)
{
	return g_strdup(purple_buddy_get_group((PurpleBuddy*)who)->name);
}

const gchar *sipe_backend_buddy_get_status(struct sipe_core_public *sipe_public,
					   const gchar *uri)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;
	PurpleBuddy *pbuddy = purple_find_buddy(purple_private->account, uri);
	const PurplePresence *presence = purple_buddy_get_presence(pbuddy);
	const PurpleStatus *pstatus = purple_presence_get_active_status(presence);
	return(purple_status_get_id(pstatus));
}

void sipe_backend_buddy_set_alias(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				  const sipe_backend_buddy who,
				  const gchar *alias)
{
	purple_blist_alias_buddy(who, alias);
}

void sipe_backend_buddy_set_server_alias(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
					 const sipe_backend_buddy who,
					 const gchar *alias)
{
	purple_blist_server_alias_buddy(who, alias);
}

gchar* sipe_backend_buddy_get_string(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				     sipe_backend_buddy buddy,
				     const sipe_buddy_info_fields key)
{
	PurpleBuddy *b = (PurpleBuddy*) buddy;
	return g_strdup(purple_blist_node_get_string(&b->node, sipe_buddy_info_to_purple_property(key)));
}

void sipe_backend_buddy_set_string(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				   sipe_backend_buddy buddy,
				   const sipe_buddy_info_fields key,
				   const gchar *val)
{
	PurpleBuddy *b = (PurpleBuddy*) buddy;
	purple_blist_node_set_string(&b->node, sipe_buddy_info_to_purple_property(key), val);
}

sipe_backend_buddy sipe_backend_buddy_add(struct sipe_core_public *sipe_public,
					  const gchar *name,
					  const gchar *alias,
					  const gchar *groupname)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;
	PurpleBuddy *b;
	PurpleGroup *purple_group = purple_find_group(groupname);

	if (!purple_group)
		return NULL;

	b = purple_buddy_new(purple_private->account, name, alias);
	purple_blist_add_buddy(b, NULL, purple_group, NULL);
	return b;
}

void sipe_backend_buddy_remove(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
			       const sipe_backend_buddy who)
{
	purple_blist_remove_buddy(who);
}

void sipe_backend_buddy_request_authorization(struct sipe_core_public *sipe_public,
					      const gchar *who,
					      const gchar *alias,
					      gboolean on_list,
					      sipe_backend_buddy_request_authorization_cb auth_cb,
					      sipe_backend_buddy_request_authorization_cb deny_cb,
					      gpointer data)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	purple_account_request_authorization(
		purple_private->account,
		who,
		_("you"), /* id */
		alias,
		NULL, /* message */
		on_list,
		auth_cb,
		deny_cb,
		data);

}

void sipe_backend_buddy_request_add(struct sipe_core_public *sipe_public,
				    const gchar *who,
				    const gchar *alias)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	purple_account_request_add(purple_private->account,
				   who,
				   _("you"),
				   alias,
				   NULL);

}

gboolean sipe_backend_buddy_is_blocked(struct sipe_core_public *sipe_public,
				       const gchar *who)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	return !purple_privacy_check(purple_private->account, who);
}

void sipe_backend_buddy_set_blocked_status(struct sipe_core_public *sipe_public,
				      const gchar *who,
				      gboolean blocked)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	if (blocked) {
		purple_privacy_deny_add(purple_private->account, who, TRUE);
	} else {
		purple_privacy_deny_remove(purple_private->account, who, TRUE);
	}

	/* stupid workaround to make pidgin re-render screen to reflect our changes */
	SIPE_DEBUG_INFO_NOFORMAT("sipe_backend_buddy_set_blocked_status: forcefully refreshing screen.");
	sipe_core_buddy_got_status(sipe_public,
				   who,
				   sipe_backend_buddy_get_status(sipe_public,
								 who));
}

void sipe_backend_buddy_set_status(struct sipe_core_public *sipe_public,
				   const gchar *who,
				   const gchar *status_id)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	purple_prpl_got_user_status(purple_private->account, who, status_id, NULL);
}

gboolean sipe_backend_buddy_group_add(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				      const gchar *group_name)
{
	PurpleGroup * purple_group = purple_find_group(group_name);
	if (!purple_group) {
		purple_group = purple_group_new(group_name);
		purple_blist_add_group(purple_group, NULL);
	}

	return (purple_group != NULL);
}

struct sipe_backend_buddy_info *sipe_backend_buddy_info_start(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public)
{
	return((struct sipe_backend_buddy_info *)purple_notify_user_info_new());
}

void sipe_backend_buddy_info_add(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				 struct sipe_backend_buddy_info *info,
				 const gchar *description,
				 const gchar *value)
{
#if PURPLE_VERSION_CHECK(3,0,0)
	purple_notify_user_info_add_pair_html
#else
	purple_notify_user_info_add_pair
#endif
		((PurpleNotifyUserInfo *) info, description, value);
}

void sipe_backend_buddy_info_break(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				   struct sipe_backend_buddy_info *info)
{
	purple_notify_user_info_add_section_break((PurpleNotifyUserInfo *) info);
}

void sipe_backend_buddy_info_finalize(struct sipe_core_public *sipe_public,
				      struct sipe_backend_buddy_info *info,
				      const gchar *uri)
{
	struct sipe_backend_private *purple_private = sipe_public->backend_private;

	/* show a buddy's user info in a nice dialog box */
	purple_notify_userinfo(purple_private->gc,
			       uri,       /* buddy's URI */
			       (PurpleNotifyUserInfo *) info,
			       NULL,      /* callback called when dialog closed */
			       NULL);     /* userdata for callback */
}

void sipe_backend_buddy_tooltip_add(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
				    struct sipe_backend_buddy_tooltip *tooltip,
				    const gchar *description,
				    const gchar *value)
{
#if PURPLE_VERSION_CHECK(3,0,0)
	purple_notify_user_info_add_pair_html
#else
	purple_notify_user_info_add_pair
#endif
		((PurpleNotifyUserInfo *) tooltip, description, value);
}

void sipe_purple_add_buddy(PurpleConnection *gc,
			   PurpleBuddy *buddy,
			   PurpleGroup *group)
{
	SIPE_DEBUG_INFO("sipe_purple_add_buddy[CB]: buddy:%s group:%s",
			buddy ? buddy->name : "",
			group ? group->name : "");

	/* libpurple can call us with undefined buddy or group */
	if (buddy && group) {
		/*
		 * Buddy name must be lower case as we use
		 * purple_normalize_nocase() to compare
		 */
		gchar *buddy_name = g_ascii_strdown(buddy->name, -1);
		gchar *uri        = sip_uri(buddy_name);
		g_free(buddy_name);
		purple_blist_rename_buddy(buddy, uri);
		g_free(uri);

		sipe_core_buddy_add(PURPLE_GC_TO_SIPE_CORE_PUBLIC,
				    buddy->name,
				    group->name);
	}
}

void sipe_purple_remove_buddy(PurpleConnection *gc,
			      PurpleBuddy *buddy,
			      PurpleGroup *group)
{
	SIPE_DEBUG_INFO("sipe_purple_remove_buddy[CB]: buddy:%s group:%s", buddy ? buddy->name : "", group ? group->name : "");
	if (!buddy) return;

	sipe_core_buddy_remove(PURPLE_GC_TO_SIPE_CORE_PUBLIC,
			       buddy->name,
			       group ? group->name : NULL);
}

void sipe_purple_group_buddy(PurpleConnection *gc,
			     const char *who,
			     const char *old_group_name,
			     const char *new_group_name)
{
	sipe_core_buddy_group(PURPLE_GC_TO_SIPE_CORE_PUBLIC, who, old_group_name, new_group_name);
}

/* Buddy Menu Handling */

static void sipe_purple_buddy_make_chat_leader_cb(PurpleBuddy *buddy,
						  gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_make_chat_leader_cb: buddy->name=%s",
			buddy->name);
	sipe_core_conf_make_leader(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
				   parameter,
				   buddy->name);
}

static void sipe_purple_buddy_remove_from_chat_cb(PurpleBuddy *buddy,
						  gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_remove_from_chat_cb: buddy->name=%s",
			buddy->name);
	sipe_core_conf_remove_from(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
				   parameter,
				   buddy->name);
}

static void sipe_purple_buddy_invite_to_chat_cb(PurpleBuddy *buddy,
						gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_invite_to_chat_cb: buddy->name=%s",
			buddy->name);
	sipe_core_chat_invite(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
			      parameter,
			      buddy->name);
}

static void sipe_purple_buddy_new_chat_cb(PurpleBuddy *buddy,
					  SIPE_UNUSED_PARAMETER gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_new_chat_cb: buddy->name=%s",
			buddy->name);
	sipe_core_buddy_new_chat(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
				 buddy->name);
}

static void sipe_purple_buddy_make_call_cb(PurpleBuddy *buddy,
					   gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_make_call_cb: buddy->name=%s", buddy->name);
	sipe_core_buddy_make_call(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
				  parameter);
}

static void sipe_purple_buddy_send_email_cb(PurpleBuddy *buddy,
					    SIPE_UNUSED_PARAMETER gpointer parameter)
{
	SIPE_DEBUG_INFO("sipe_purple_buddy_send_email_cb: buddy->name=%s",
			buddy->name);
	sipe_core_buddy_send_email(PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC,
				   buddy->name);
}

typedef void (*buddy_menu_callback)(PurpleBuddy *buddy,
				    gpointer parameter);
static const buddy_menu_callback callback_map[SIPE_BUDDY_MENU_TYPES] = {
	/* SIPE_BUDDY_MENU_MAKE_CHAT_LEADER */ sipe_purple_buddy_make_chat_leader_cb,
	/* SIPE_BUDDY_MENU_REMOVE_FROM_CHAT */ sipe_purple_buddy_remove_from_chat_cb,
	/* SIPE_BUDDY_MENU_INVITE_TO_CHAT   */ sipe_purple_buddy_invite_to_chat_cb,
	/* SIPE_BUDDY_MENU_NEW_CHAT         */ sipe_purple_buddy_new_chat_cb,
	/* SIPE_BUDDY_MENU_MAKE_CALL        */ sipe_purple_buddy_make_call_cb,
	/* SIPE_BUDDY_MENU_SEND_EMAIL       */ sipe_purple_buddy_send_email_cb,
};

struct sipe_backend_buddy_menu *sipe_backend_buddy_menu_start(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public)
{
	return(NULL);
}

struct sipe_backend_buddy_menu *sipe_backend_buddy_menu_add(SIPE_UNUSED_PARAMETER struct sipe_core_public *sipe_public,
							    struct sipe_backend_buddy_menu *menu,
							    const gchar *label,
							    enum sipe_buddy_menu_type type,
							    gpointer parameter)
{
	return((struct sipe_backend_buddy_menu *)
	       g_list_prepend((GList *) menu,
			      purple_menu_action_new(label,
						     PURPLE_CALLBACK(callback_map[type]),
						     parameter, NULL)));
}

static void sipe_purple_buddy_copy_to_cb(PurpleBlistNode *node,
					 const gchar *group_name)
{
	struct sipe_core_public *sipe_public;
	PurpleBuddy *buddy = (PurpleBuddy *)node;
	PurpleGroup *group;
	PurpleBuddy *clone;

	g_return_if_fail(PURPLE_BLIST_NODE_IS_BUDDY(node));

	sipe_public = PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC;
	group       = purple_find_group(group_name);

	SIPE_DEBUG_INFO("sipe_purple_buddy_copy_to_cb: copying %s to %s",
			buddy->name, group_name);

	clone = purple_find_buddy_in_group(buddy->account, buddy->name, group);
	if (!clone) {
		clone = sipe_backend_buddy_add(sipe_public,
					       buddy->name,
					       buddy->alias,
					       group->name);
		if (clone) {
			const gchar *tmp;
			const gchar *key;
			const PurpleStatus *status = purple_presence_get_active_status(purple_buddy_get_presence(buddy));

			tmp = purple_buddy_get_server_alias(buddy);
			if (tmp) purple_blist_server_alias_buddy(clone, tmp);

			key = sipe_buddy_info_to_purple_property(SIPE_BUDDY_INFO_EMAIL);
			tmp = purple_blist_node_get_string(&buddy->node, key);
			if (tmp) purple_blist_node_set_string(&clone->node,
							      key,
							      tmp);

			tmp = purple_status_get_id(status);
			purple_presence_set_status_active(purple_buddy_get_presence(clone),
							  tmp,
							  TRUE);

			/* update UI */
			purple_prpl_got_user_status(clone->account,
						    clone->name,
						    tmp,
						    NULL);
		}
	}

	if (clone && group)
		sipe_core_buddy_add(sipe_public,
				    clone->name,
				    group->name);
}

static GList *sipe_purple_copy_to_menu(GList *menu,
				       PurpleBuddy *buddy)
{
	GList *menu_groups = NULL;
	PurpleGroup *gr_parent = purple_buddy_get_group(buddy);
	PurpleBlistNode *g_node;

	for (g_node = purple_blist_get_root(); g_node; g_node = g_node->next) {
		PurpleGroup *group = (PurpleGroup *)g_node;
		PurpleMenuAction *act;

		if ((g_node->type != PURPLE_BLIST_GROUP_NODE) ||
		    (group == gr_parent)                      ||
		    purple_find_buddy_in_group(buddy->account,
					       buddy->name,
					       group))
			continue;

		act = purple_menu_action_new(purple_group_get_name(group),
					     PURPLE_CALLBACK(sipe_purple_buddy_copy_to_cb),
					     group->name, NULL);
		menu_groups = g_list_prepend(menu_groups, act);
	}

	if (menu_groups)
		menu = g_list_prepend(menu,
				      purple_menu_action_new(_("Copy to"),
							     NULL,
							     NULL,
							     g_list_reverse(menu_groups)));

	return(menu);
}

GList *sipe_purple_buddy_menu(PurpleBuddy *buddy)
{
	struct sipe_core_public *sipe_public = PURPLE_BUDDY_TO_SIPE_CORE_PUBLIC;
	GList *menu;

	/*
	 * Workaround for missing libpurple API to release resources allocated
	 * during blist_node_menu() callback. See also:
	 *
	 *   <http://developer.pidgin.im/ticket/12597>
	 *
	 * We remember all memory blocks in a list and deallocate them when
	 *
	 *   - the next time we enter the callback, or
	 *   - the account is disconnected
	 *
	 * That means that after the buddy menu has been closed we have unused
	 * resources but at least we don't leak them anymore...
	 */
	sipe_core_buddy_menu_free(sipe_public);

	/* now create the new menu... */
	menu = (GList *) sipe_core_buddy_create_menu(sipe_public,
						     buddy->name,
						     NULL);
	menu = sipe_purple_copy_to_menu(menu, buddy);

	return(g_list_reverse(menu));
}


/*
  Local Variables:
  mode: c
  c-file-style: "bsd"
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
