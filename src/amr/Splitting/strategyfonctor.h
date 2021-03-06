#ifndef STRATEGYFONCTOR_H
#define STRATEGYFONCTOR_H

#include <memory>

#include "amr/Splitting/approx_1to4strategy.h"
#include "amr/Splitting/approx_fujistrategy.h"
#include "amr/Splitting/order1_rf2strategy.h"
#include "amr/Splitting/order1_rfnstrategy.h"
#include "amr/Splitting/order2_rf2strategy.h"
#include "amr/Splitting/order3_rf2strategy.h"
#include "amr/Splitting/ordern_rf2strategy.h"
#include "amr/Splitting/ordern_rf3strategy.h"
#include "amr/Splitting/splittingstrategy.h"



class StrategyFunctor
{
protected:
    std::string strategy_;
    uint32 refineFactor_;
    uint32 interpOrder_;

public:
    StrategyFunctor(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : strategy_{strategy}
        , refineFactor_{refineFactor}
        , interpOrder_{interpOrder}
    {
    }

    virtual ~StrategyFunctor() = default;

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const = 0;

    uint32 refineFactor() const { return refineFactor_; }
    uint32 interpOrder() const { return interpOrder_; }
};




class FonctorApprox1to4 : public StrategyFunctor
{
public:
    explicit FonctorApprox1to4(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Approx_1to4Strategy(strategy_));
    }
};




class FonctorApproxFuji : public StrategyFunctor
{
public:
    FonctorApproxFuji(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Approx_FujiStrategy(strategy_));
    }
};




class FonctorOrder1_RF2 : public StrategyFunctor
{
public:
    FonctorOrder1_RF2(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Order1_RF2Strategy(strategy_));
    }
};




class FonctorOrder2_RF2 : public StrategyFunctor
{
public:
    FonctorOrder2_RF2(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Order2_RF2Strategy(strategy_));
    }
};




class FonctorOrder3_RF2 : public StrategyFunctor
{
public:
    FonctorOrder3_RF2(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Order3_RF2Strategy(strategy_));
    }
};




class FonctorOrder1_RFn : public StrategyFunctor
{
public:
    FonctorOrder1_RFn(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new Order1_RFnStrategy(strategy_, refineFactor_));
    }
};




class FonctorOrderN_RF2 : public StrategyFunctor
{
public:
    FonctorOrderN_RF2(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new OrderN_RF2Strategy(strategy_, interpOrder_));
    }
};




class FonctorOrderN_RF3 : public StrategyFunctor
{
public:
    FonctorOrderN_RF3(std::string const& strategy, uint32 refineFactor, uint32 interpOrder)
        : StrategyFunctor(strategy, refineFactor, interpOrder)
    {
    }

    virtual std::unique_ptr<SplittingStrategy> createStrategy() const override
    {
        return std::unique_ptr<SplittingStrategy>(new OrderN_RF3Strategy(strategy_, interpOrder_));
    }
};



#endif // STRATEGYFONCTOR_H
