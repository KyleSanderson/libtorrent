#include "config.h"

#include <tr1/functional>

#include "data/hash_queue_node.h"
#include "torrent/chunk_manager.h"
#include "torrent/exceptions.h"
#include "torrent/hash_string.h"
#include "torrent/poll_select.h"
#include "torrent/utils/thread_base_test.h"
#include "thread_disk.h"

#include "chunk_list_test.h"
#include "hash_queue_test.h"
#include "hash_check_queue_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION(HashQueueTest);

namespace tr1 { using namespace std::tr1; }

typedef std::map<int, torrent::HashString> done_chunks_type;

static void
chunk_done(done_chunks_type* done_chunks, torrent::ChunkHandle handle, const char* hash_value) {
  (*done_chunks)[handle.index()] = *torrent::HashString::cast_from(hash_value);
}

static torrent::Poll* create_select_poll() { return torrent::PollSelect::create(256); }

static void do_nothing() {}

void
HashQueueTest::setUp() {
  torrent::Poll::slot_create_poll() = tr1::bind(&create_select_poll);

  signal(SIGUSR1, (sig_t)&do_nothing);
}

void
HashQueueTest::tearDown() {
}

void
HashQueueTest::test_basic() {
  // SETUP_CHUNK_LIST();
  // SETUP_THREAD();
  // thread_disk->start_thread();

  // torrent::HashQueue* hash_queue = new torrent::HashQueue(thread_disk);
  
  // // Do stuff?

  // delete hash_queue;

  // thread_disk->stop_thread();
  // CLEANUP_THREAD();
  // CLEANUP_CHUNK_LIST();
}

void
HashQueueTest::test_single() {
  SETUP_CHUNK_LIST();
  SETUP_THREAD();
  thread_disk->start_thread();

  torrent::HashQueue* hash_queue = new torrent::HashQueue(thread_disk);

  done_chunks_type done_chunks;

  torrent::ChunkHandle handle_0 = chunk_list->get(0, torrent::ChunkList::get_blocking);
  hash_queue->push_back(handle_0, NULL, tr1::bind(&chunk_done, &done_chunks, tr1::placeholders::_1, tr1::placeholders::_2));
  
  CPPUNIT_ASSERT(hash_queue->size() == 1);
  CPPUNIT_ASSERT(hash_queue->front().handle().is_blocking());
  CPPUNIT_ASSERT(hash_queue->front().handle().object() == &((*chunk_list)[0]));

  hash_queue->work();

  CPPUNIT_ASSERT(done_chunks.find(0) != done_chunks.end());
  CPPUNIT_ASSERT(done_chunks[0] == hash_for_index(0));

  chunk_list->release(&handle_0);
  
  delete hash_queue;

  thread_disk->stop_thread();
  CLEANUP_THREAD();
  CLEANUP_CHUNK_LIST();
}
