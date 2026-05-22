#include <cxfunc/stats.hpp>
#include <iostream>
#include <iomanip>

void onoff_test() {
    using namespace cxfunc::stats;

    // 观测参数
    double n_on = 24.0;    // ON区域观测到的计数
    double n_off = 110.0;  // OFF区域观测到的计数
    double alpha = 0.05;   // ON/OFF接受度比率

    std::cout << "========================================" << std::endl;
    std::cout << "ON-OFF 观测统计显著性计算" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "输入参数:" << std::endl;
    
    std::cout << "  OFF 计数 (n_off): " << n_off << std::endl;
    std::cout << "  Alpha (α):        " << alpha << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 创建W统计对象
    WStatCountsStatistic wstat(n_on, n_off, alpha);

    std::cout << std::setprecision(6);
    std::cout << "计算结果:" << std::endl;
    std::cout << "  背景 (α × n_off):       " << wstat.n_bkg() << std::endl;
    std::cout << "  超额 (n_on - bkg):     " << wstat.excess() << std::endl;
    std::cout << "  TS (检验统计量):        " << wstat.ts() << std::endl;
    std::cout << "  √TS (显著性):          " << wstat.sqrt_ts() << " sigma" << std::endl;
    std::cout << "  p值:                   " << wstat.p_value() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 计算误差
    double errn = wstat.compute_errn(1.0);  // 1σ 向下误差
    double errp = wstat.compute_errp(1.0);  // 1σ 向上误差
    double ul = wstat.compute_upper_limit(3.0);  // 3σ 上限

    std::cout << "误差估计:" << std::endl;
    std::cout << "  1σ 下行误差:           -" << errn << std::endl;
    std::cout << "  1σ 上行误差:           +" << errp << std::endl;
    std::cout << "  3σ 上限:               " << ul << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 物理意义解释
    std::cout << "物理意义:" << std::endl;
    std::cout << "  预期背景: " << wstat.n_bkg() << " 计数" << std::endl;
    std::cout << "  观测到:   " << n_on << " 计数" << std::endl;
    std::cout << "  超出:     " << wstat.excess() << " 计数" << std::endl;
    std::cout << "  统计显著性: " << wstat.sqrt_ts() << " σ" << std::endl;

    if (std::abs(wstat.sqrt_ts()) >= 5.0) {
        std::cout << "  结论: 发现 (≥5σ)" << std::endl;
    } else if (std::abs(wstat.sqrt_ts()) >= 3.0) {
        std::cout << "  结论: 证据 (3-5σ)" << std::endl;
    } else if (std::abs(wstat.sqrt_ts()) >= 2.0) {
        std::cout << "  结论: 迹象 (2-3σ)" << std::endl;
    } else {
        std::cout << "  结论: 不显著 (<2σ)" << std::endl;
    }

    std::cout << "========================================" << std::endl;

}

int main() {
    onoff_test();
}
