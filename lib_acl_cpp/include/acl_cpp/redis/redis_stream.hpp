#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "redis_command.hpp"

namespace acl
{

class redis_client;
class redis_client_cluster;

struct redis_stream_field
{
	string name;
	string value;
};

struct redis_stream_message
{
	string id;
	std::vector<redis_stream_field> fields;
};

struct redis_stream_messages
{
	string key;
	std::map<string, redis_stream_message> messages;
};

struct redis_stream_info
{
	size_t length;
	size_t radix_tree_keys;
	size_t radix_tree_nodes;
	size_t groups;
	string last_generated_id;
	redis_stream_message first_entry;
	redis_stream_message last_entry;
};

class ACL_CPP_API redis_stream : virtual public redis_command
{
public:
	redis_stream(void);
	redis_stream(redis_client* conn);
	redis_stream(redis_client_cluster* cluster, size_t max_conns = 0);

	virtual ~redis_stream(void);

	/////////////////////////////////////////////////////////////////////

	bool xadd(const char* key, const std::map<string, string>& fields,
		string& result, const char* id = "*");
	bool xadd(const char* key, const std::vector<string>& names,
		const std::vector<string>& values,
		string& result, const char* id = "*");
	bool xadd(const char* key, const std::vector<const char*>& names,
		const std::vector<const char*>& values,
		string& result, const char* id = "*");
	bool xadd(const char* key, const char* names[], const size_t names_len[],
		const char* values[], const size_t values_len[], size_t argc,
		string& result, const char* id = "*");

	int  xlen(const char* key);
	int  xdel(const char* key, const std::vector<string>& ids);
	int  xdel(const char* key, const std::vector<const char*>& ids);
	int  xtrim(const char* key, size_t maxlen, bool tilde = false);

	/////////////////////////////////////////////////////////////////////

	bool xread(redis_stream_messages& messages,
		const std::map<string, string>& streams,
		size_t count = 0, size_t block = 0);
	bool xreadgroup(redis_stream_messages& messsages,
		const char* group, const char* consumer,
		const std::map<string, string>& streams,
		size_t count = 0, size_t block = 0);
	bool xrange(redis_stream_messages& messages, const char* key,
		const char* start = "-", const char* end = "+", size_t count = 0);
	bool xrevrange(redis_stream_messages& messages, const char* key,
		const char* start = "+", const char* end = "-", size_t count = 0);

	/////////////////////////////////////////////////////////////////////

	int  xack(const char* key, const char* group, const char* id);
	int  xack(const char* key, const char* group,
		const std::vector<string>& ids);
	int  xack(const char* key, const char* group,
		const std::vector<const char*>& ids);
	int  xack(const char* key, const char* group,
		const std::list<string>& ids, size_t size);
	int  xack(const char* key, const char* group,
		const std::list<const char*>& ids, size_t size);

	/////////////////////////////////////////////////////////////////////

	bool xgroup_create(const char* key, const char* group,
		const char* id = "$");
	int  xgroup_destroy(const char* key, const char* group);
	bool xgroup_setid(const char* key, const char* group,
		const char* id = "$");
	int  xgroup_delconsumer(const char* key, const char* group,
		const char* consumer);

	/////////////////////////////////////////////////////////////////////

	bool xinfo_help(std::vector<string>& result);
	bool xinfo_stream(const char* key, redis_stream_info& result);

	/////////////////////////////////////////////////////////////////////

private:
	void build(const char* cmd, const char* key, const char* id,
		const std::map<string, string>& fields);
	void build(const char* cmd, const char* key, const char* id,
		const std::vector<string>& names,
		const std::vector<string>& values);
	void build(const char* cmd, const char* key, const char* id,
		const std::vector<const char*>& names,
		const std::vector<const char*>& values);
	void build(const char* cmd, const char* key, const char* id,
		const char* names[], const size_t names_len[],
		const char* values[], const size_t values_len[], size_t argc);
	void build(const std::map<string, string>& streams, size_t count,
		size_t block, size_t i);
	void xread_build(const std::map<string, string>& streams,
		size_t count, size_t block);
	void xreadgroup_build(const char* group, const char* consumer,
		const std::map<string, string>& streams,
		size_t count, size_t block);
	bool get_results(redis_stream_messages& messages);
	bool get_messages(const redis_result& rr, redis_stream_messages& messages);
	bool get_one_message(const redis_result& rr, redis_stream_message& message);
	bool get_one_field(const redis_result& rr, redis_stream_field& field);
	bool range(redis_stream_messages& messages, const char* cmd,
	     	const char* key, const char* start, const char* end, size_t count);
};

}