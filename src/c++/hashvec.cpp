
#include "hashvec.h"

HashVec::HashVec() : iomode_(std::getenv("PROF_IO_MODE")), format_(std::getenv("PROF_OUT_FORMAT")) {}

std::vector<std::pair<size_t, HashEntry>>& HashVec::get()
{
    return hashvec_;
}

void HashVec::sort()
{
    std::sort
    (
        begin(hashvec_), end(hashvec_),
        [] (auto a, auto b) { 
            return a.second.self_walltime_ > b.second.self_walltime_; 
        }
    );
}

void HashVec::write()
{
    if ( format_ == NULL && iomode_ == NULL )
    {
        std::make_unique<Standard>()->accept(std::make_unique<Multifile>(), get());
    }
    else throw std::runtime_error("Invalid PROF_IO_MODE choice");
}