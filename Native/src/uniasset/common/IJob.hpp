#ifndef UNIASSET_IJOB_HPP
#define UNIASSET_IJOB_HPP

namespace uniasset {
    class IJob {
    public:
        virtual ~IJob() = default;

        virtual bool hasWork() = 0;

        virtual void execute() = 0;
    };
}

#endif // UNIASSET_IJOB_HPP
