#
# producer/consumer
#

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(producer_consumer src/main.cc)
target_link_libraries(producer_consumer PRIVATE Threads::Threads)
install(TARGETS producer_consumer)
