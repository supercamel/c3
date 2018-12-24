/*

   Copyright 2017 Samuel Cowen <samuel.cowen@camelsoftware.com>

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

*/

#ifndef C3_H_INCLUDED
#define C3_H_INCLUDED

#include <stdint.h>
#include <etk.h>
#include "c3_enums.h"
#include "c3_settings.h"

namespace ccc
{
    constexpr uint16 MAX_PACKET_LENGTH = 64;
	constexpr uint32 POOL_SIZE = 256;

	union u32b
	{
		uint32 u;
		int32 i;
		float f;
		uint8 bytes[4];
	};

	union u16b
	{
		uint16 u;
		int16 i;
		uint8 bytes[2];
	};

	class base_pack
	{
		public:
			base_pack(uint8 sync) : sync(sync) { }
			virtual uint16 get_id() = 0;
			virtual uint16 to_bytes(uint8* bytes) = 0;
			virtual void from_bytes(uint8* bytes) = 0;
			virtual bool ack_required() = 0;
			virtual uint8 size() = 0;

			uint8 get_sync()
			{
				return sync;
			}

			void gen_header(uint8* bytes)
			{
				bytes[0] = get_sync();
				u16b u;
				u.u = get_id();
				bytes[1] = u.bytes[0];
				bytes[2] = u.bytes[1];
				bytes[3] = size();
			}


			template <uint32 SZ> void to_bytes(uint8* bytes, uint16& pos, etk::StaticString<SZ>& str)
			{
				if(pos > MAX_PACKET_LENGTH-SZ)
					return;

				for(uint32 i = 0; i < SZ; i++)
				{
					bytes[pos++] = str[i];
				}
			}

			template <uint16 SZ> void to_bytes(uint8* bytes, uint16& pos, etk::List<uint8, SZ>& lst)
			{
				uint8 sz = lst.size();
				if(pos > MAX_PACKET_LENGTH-sz)
					return;
				bytes[pos++] = sz;

				for(auto i : etk::range(sz))
				{
					bytes[pos++] = lst.get(i);
				}
			}

			void to_bytes(uint8* bytes, uint16& pos, float f)
			{
			    if(pos > MAX_PACKET_LENGTH-4) 
			        return;
				u32b ub;
				ub.f = f;
				for(uint8 i = 0; i < 4; i++) {
					bytes[pos++] = ub.bytes[i];
				}
			}
			
			void to_bytes(uint8* bytes, uint16& pos, uint8 u)
			{
			    if(pos > MAX_PACKET_LENGTH-1)
			        return;
			    bytes[pos++] = u;
			}
			
			void to_bytes(uint8* bytes, uint16& pos, int8 i)
			{
			    if(pos > MAX_PACKET_LENGTH-1)
			        return;
			    bytes[pos++] = i;
			}
			
			void to_bytes(uint8* bytes, uint16& pos, uint16 u)
			{
			    if(pos > MAX_PACKET_LENGTH-2)
			        return;
			    
			    u16b ub;
			    ub.u = u;
			    bytes[pos++] = ub.bytes[0];
			    bytes[pos++] = ub.bytes[1];
			}
			
			void to_bytes(uint8* bytes, uint16& pos, int16 i)
			{
			    if(pos > MAX_PACKET_LENGTH-2)
			        return;
			    u16b ub;
			    ub.i = i;
			    bytes[pos++] = ub.bytes[0];
			    bytes[pos++] = ub.bytes[1];
			}
			
			void to_bytes(uint8* bytes, uint16& pos, uint32 u)
			{
			    if(pos > MAX_PACKET_LENGTH-4)
			        return;
			    
			    u32b ub;
			    ub.u = u;
			    bytes[pos++] = ub.bytes[0];
			    bytes[pos++] = ub.bytes[1];
			    bytes[pos++] = ub.bytes[2];
			    bytes[pos++] = ub.bytes[3];
			}
			
			void to_bytes(uint8* bytes, uint16& pos, int32 i)
			{
			    if(pos > MAX_PACKET_LENGTH-4)
			        return;
			    u32b ub;
			    ub.i = i;
			    bytes[pos++] = ub.bytes[0];
			    bytes[pos++] = ub.bytes[1];
			    bytes[pos++] = ub.bytes[2];
			    bytes[pos++] = ub.bytes[3];
			}

/*
			template<typename T, typename... Args> void to_bytes(uint8* bytes, uint16& pos, T t, Args ... args)
			{
				if(pos > 250)
					return;

                if(sizeof(T) == 1) {
                    to_bytes(bytes, pos, static_cast<uint8>(t));
                } else {
				    to_bytes(bytes, pos, t);
				}
				to_bytes(bytes, pos, args...);
			}
*/
			template <uint32 SZ> void from_bytes(uint8* bytes, uint16& pos, etk::StaticString<SZ>& str)
			{
				for(uint32 i = 0; i < SZ; i++)
				{
					str[i] = bytes[pos++];
				}
			}

			template <uint16 SZ> void from_bytes(uint8* bytes, uint16& pos, etk::List<uint8, SZ>& lst)
			{
				uint8 sz = bytes[pos++];

				for(uint32 i = 0; i < sz; i++)
				{
					lst.append(bytes[pos++]);
				}
			}

			void from_bytes(uint8* bytes, uint16& pos, float& f)
			{
				u32b ub;
				ub.bytes[0] = bytes[pos++];
				ub.bytes[1] = bytes[pos++];
				ub.bytes[2] = bytes[pos++];
				ub.bytes[3] = bytes[pos++];

				f = ub.f;
			}
			
			void from_bytes(uint8* bytes, uint16& pos, uint8& u)
			{
			    u = bytes[pos++];
			}
			
			void from_bytes(uint8* bytes, uint16& pos, int8& i)
			{
			    i = bytes[pos++];
			}
			
			void from_bytes(uint8* bytes, uint16& pos, uint16& u)
			{
			    u16b ub;
			    ub.bytes[0] = bytes[pos++];
			    ub.bytes[1] = bytes[pos++];
			    u = ub.u;
			}
			
			void from_bytes(uint8* bytes, uint16& pos, int16& i)
			{
			    u16b ub;
			    ub.bytes[0] = bytes[pos++];
			    ub.bytes[1] = bytes[pos++];
			    i = ub.i;
			}
			
			void from_bytes(uint8* bytes, uint16& pos, uint32& u)
			{
			    u32b ub;
			    ub.bytes[0] = bytes[pos++];
			    ub.bytes[1] = bytes[pos++];
			    ub.bytes[2] = bytes[pos++];
			    ub.bytes[3] = bytes[pos++];
			    u = ub.u;
			}
			
			void from_bytes(uint8* bytes, uint16& pos, int32& i)
			{
			    u32b ub;
			    ub.bytes[0] = bytes[pos++];
			    ub.bytes[1] = bytes[pos++];
			    ub.bytes[2] = bytes[pos++];
			    ub.bytes[3] = bytes[pos++];
			    i = ub.i;
			}
/*
			template<typename T, typename... Args> void from_bytes(uint8* bytes, uint16& pos, T& t, Args&& ... args)
			{
			    if(sizeof(T) == 1) {
			        uint8 u;
			        from_bytes(bytes, pos, u);
			        t = u;
			    } else {
				    from_bytes(bytes, pos, t);
				}
				from_bytes(bytes, pos, args...);
			}
			*/

		private:
			uint8 sync;
	};

	class ack_pack : public base_pack
	{
		public:
			ack_pack(uint8 sync) : base_pack(sync)  { }

			static constexpr uint16 id = 0;
			uint16 msg_id = 0;
			uint8 sync = 0;

			uint8 size()
			{
				return 3;
			}

			bool ack_required() {
				return false;
			}

			uint16 get_id()
			{
				return id;
			}

			uint16 to_bytes(uint8* bytes)
			{
				uint16 pos = 4;
				base_pack::gen_header(bytes);
				base_pack::to_bytes(bytes, pos, msg_id);
				base_pack::to_bytes(bytes, pos, sync);
				return pos;
			}

			void from_bytes(uint8* bytes)
			{
				uint16 pos = 0;
				base_pack::from_bytes(bytes, pos, msg_id);
				base_pack::from_bytes(bytes, pos, sync);
			}
	};

#define C3_PULLS_IN_PACKETS
#include "c3_packets.h"
#undef C3_PULLS_IN_PACKETS

	struct REMOTE { };
	struct LOCAL { };

	template<typename T, typename OWNER> class C3Link
	{
		public:
			C3Link() : list(pool)
		{
		}

			void read()
			{
			    while(static_cast<T*>(this)->available())
			    {
			        uint8 c = static_cast<T*>(this)->get();
				    switch(state)
				    {
					    case MSG_STATE_START_1:
						    {
							    data_pos = 0;
							    if(c == 0xAB)
								    state = MSG_STATE_START_2;
						    }
						    break;
					    case MSG_STATE_START_2:
						    {
							    data_pos = 0;
							    if(c == 0xCD)
								    state = MSG_STATE_SYN;
							    else
								    state = MSG_STATE_START_1;
						    }
						    break;
					    case MSG_STATE_SYN:
						    {
							    sync_in = c;
							    add_byte(c);
							    state = MSG_STATE_ID_1;
						    }
						    break;
					    case MSG_STATE_ID_1:
						    {
							    msg_id = c;
							    add_byte(c);
							    state = MSG_STATE_ID_2;
						    }
						    break;
					    case MSG_STATE_ID_2:
						    {
							    msg_id += (c << 8);
							    if(msg_id >= NULL_PACK)
								    state = MSG_STATE_START_1;
							    else
							    {
								    add_byte(c);
								    state = MSG_STATE_SIZE;
							    }
						    }
						    break;
					    case MSG_STATE_SIZE:
						    {
							    add_byte(c);
							    data_length = c;
							    if(data_length == 0) {
								    state = MSG_STATE_CHECK_1; 
							    }
							    else {
								    state = MSG_STATE_DATA;
							    }
						    }
						    break;
					    case MSG_STATE_DATA:
						    {
							    add_byte(c);
							    if(data_pos >= (data_length+4))
								    state = MSG_STATE_CHECK_1;
						    }
						    break;
					    case MSG_STATE_CHECK_1:
						    {
							    checksum_in = c;
							    state = MSG_STATE_CHECK_2;
						    }
						    break;
					    case MSG_STATE_CHECK_2:
						    {
							    checksum_in += (c << 8);

							    if(check_checksum())
								    read_data<void>(OWNER());

							    state = MSG_STATE_START_1;
						    }
						    break;
					    default:
						    state = MSG_STATE_START_1;
				    }
				}
			}

			template <typename PACK_T> PACK_T* create_packet()
			{
				void* rptr = pool.alloc(sizeof(PACK_T));
				if(rptr == nullptr)
					return nullptr;
				PACK_T* ptr = new(rptr) PACK_T(sync++);
				if(!list.append(ptr))
				{
					pool.free(rptr);
					return nullptr;
				}
				return ptr;
			}

			void serialise()
			{
				uint8 buf[MAX_PACKET_LENGTH];

				uint32 count = 0;
				for(auto& ack : ack_list)
				{
					uint16 len = ack.to_bytes(buf);

					static_cast<T*>(this)->put(static_cast<char>(0xAB));
					static_cast<T*>(this)->put(static_cast<char>(0xCD));

					for(uint32 i = 0; i < len; i++)
						static_cast<T*>(this)->put(buf[i]);

					u16b u;
					u.u = make_checksum(buf, len);
					static_cast<T*>(this)->put(u.bytes[0]);
					static_cast<T*>(this)->put(u.bytes[1]);
				}
				ack_list.clear();
				for(auto i = list.begin(); i != list.end(); i++)
				{
					uint16 len = (*i)->to_bytes(buf);

					static_cast<T*>(this)->put(static_cast<char>(0xAB));
					static_cast<T*>(this)->put(static_cast<char>(0xCD));

					for(uint32 i = 0; i < len; i++)
						static_cast<T*>(this)->put(buf[i]);

					u16b u;
					u.u = make_checksum(buf, len);
					static_cast<T*>(this)->put(u.bytes[0]);
					static_cast<T*>(this)->put(u.bytes[1]);

					if(!(*i)->ack_required())
					{
						pool.free(*i);
						list.remove(i);
					}
				}
			}

		private:
			void add_byte(uint8 c)
			{
				data_buf[data_pos++] = c;
			}

			//Flecher16 checksum algorithm
			//https://en.wikipedia.org/wiki/Fletcher%27s_checksum#Fletcher-16
			uint16 make_checksum(uint8* data, uint8 bytes)
			{
				uint16 sum1 = 0xff, sum2 = 0xff;
				size_t tlen;

				while(bytes)
				{
					tlen = ((bytes >= 20) ? 20 : bytes);
					bytes -= tlen;
					do
					{
						sum2 += sum1 += *data++;
						tlen--;
					}
					while (tlen);
					sum1 = (sum1 & 0xff) + (sum1 >> 8);
					sum2 = (sum2 & 0xff) + (sum2 >> 8);
				}
				/* Second reduction step to reduce sums to 8 bits */
				sum1 = (sum1 & 0xff) + (sum1 >> 8);
				sum2 = (sum2 & 0xff) + (sum2 >> 8);
				uint16 checksum = (sum2 << 8) | sum1;
				return checksum;
			}

			bool check_checksum()
			{
				auto ncheck = make_checksum(data_buf, data_length+4);
				return (ncheck==checksum_in);
			}

			void send_ack(uint32 m, uint8 s)
			{
				ack_pack ack(sync++);
				ack.msg_id = m;
				ack.sync = s;
				ack_list.append(ack);
			}

			void ack_handler()
			{
				ack_pack ackp(sync_in);
				ackp.from_bytes(&(data_buf[4]));

				for(auto i = list.begin(); i != list.end(); i++)
				{
					base_pack* ptr = *i;
					if((ptr->get_id() == ackp.msg_id) && (ptr->get_sync() == ackp.sync))
					{
						pool.free(ptr);
						list.remove(i);
					}
				}
			}

			template <typename O> void read_data(REMOTE r)
			{
				auto& settings = Settings::get();

				switch(msg_id)
				{
					case ACK_PACK:
						ack_handler();
						break;
#define C3_PULLS_IN_REMOTE_HANDLERS
#include "c3_remote_handlers.h"
#undef C3_PULLS_IN_REMOTE_HANDLERS

#define C3_PULLS_IN_BOTH_HANDLERS
#include "c3_both_handlers.h"
#undef C3_PULLS_IN_BOTH_HANDLERS
				}
			}

			template <typename O> void read_data(LOCAL l)
			{
				auto& settings = Settings::get();

				switch(msg_id)
				{
					case ACK_PACK:
						ack_handler();
						break;
#define C3_PULLS_IN_LOCAL_HANDLERS
#include "c3_local_handlers.h"
#undef C3_PULLS_IN_LOCAL_HANDLERS

#define C3_PULLS_IN_BOTH_HANDLERS
#include "c3_both_handlers.h"
#undef C3_PULLS_IN_BOTH_HANDLERS
				}
			}


			enum MSG_STATE
			{
				MSG_STATE_START_1,
				MSG_STATE_START_2,
				MSG_STATE_SYN,
				MSG_STATE_ID_1,
				MSG_STATE_ID_2,
				MSG_STATE_SIZE,
				MSG_STATE_DATA,
				MSG_STATE_CHECK_1,
				MSG_STATE_CHECK_2
			};

			MSG_STATE state = MSG_STATE_START_1;
			uint8 sync_in = 0;
			uint16 msg_id = 0;
			uint16 checksum_in = 0;
			uint8 data_length = 0;
			uint8 data_pos = 0;

			uint8 sync = 0;
			
			uint8 data_buf[MAX_PACKET_LENGTH];

			etk::experimental::MemPool<POOL_SIZE> pool;
			etk::experimental::linked_list<base_pack*, etk::experimental::MemPool<POOL_SIZE>> list;
			etk::List<ack_pack, 4> ack_list;
	};


}

#define C3_COMPLETED

#endif
