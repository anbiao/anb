<template>
  <div id="Scroll-Board">
    <dv-scroll-board :config="config" />
  </div>
</template>

<script>
  export default {
    name: 'ScrollBoard',
    data () {
      return {
        config: {
          header: ['时间', '省份', '事件','风险类型','风险等级'],
          data: [
            ['2019-07-01','广西','交割库容严重不足，遭到空方强烈抗议维权', '价格波动风险', '<div class="risk-rate lower">1</div>'],
            ['2019-07-02','海南','市场限仓提保，企业亏损', '合同履约风险', '<div class="risk-rate low">2</div>'],
            ['2019-07-03','浙江','市场大幅提保，致诉讼变相维权', '价格波动风险', '<div class="risk-rate middle">3</div>'],
            ['2019-07-04','北京','交易商维权，市场关闭', '价格波动风险', '<div class="risk-rate higher">5</div>'],
            ['2019-07-05','北京','公安机关收到举报，市场停业赔偿', '仓单质押风险', '<div class="risk-rate middle">3</div>'],
            ['2019-07-06','山东','市场打压价格，导致停业整顿，交易商维权', '价格波动风险', '<div class="risk-rate lower">1</div>'],
            ['2019-07-07','天津','市场姑息代理刷单，价格严重背离，导致市场关闭', '价格波动风险', '<div class="risk-rate high">4</div>'],
            ['2019-07-08','湖南','恶意代客刷单，平台交易量造假，法人被抓，内部整顿', '合同履约风险', '<div class="risk-rate middle">3</div>'],
            ['2019-07-09','深圳','虚拟活跃交易，被清理整顿，市场关闭', '合同履约风险', '<div class="risk-rate middle">3</div>'],
            ['2019-07-10','江西','市场操纵价格，客户上访维权，市场关闭', '仓单质押风险', '<div class="risk-rate low">1</div>']
          ],
          columnWidth: [180, 180, 500, 200, 180],
          align: ['center', 'center', 'center', 'center', 'right'],
          waitTime: 3000,
          rowNum: 5,
          headerBGC: '#1981f6',
          oddRowBGC: 'rgba(0, 44, 81, 0.8)',
          evenRowBGC: 'rgba(10, 29, 50, 0.8)'
        }
      }
    }
  }
</script>

<style lang="less">
  #Scroll-Board {
    box-sizing: border-box;
    width: 100%;
    height: 100%;
    overflow: hidden;

    .header {
      height: auto;
    }

    .ceil {
      color: #fff;
      font-weight: 400;

      .risk-rate {
        display: inline-block;
        vertical-align: middle;
        white-space: nowrap;
        text-align: center;
        border-radius: 10px;
        box-shadow: 0 0 0 1px #ccc;
        padding: 2px 6px;
        margin: 0 8px 4px 8px;
        font-size: 10px;
        font-weight: 600;
        line-height: 12px;
      }
      .higher {
        color: #000000;
        background: #f5222d;
      }

      .high {
        color: #000000;
        background: #ff751e;
      }

      .middle {
        color: #000000;
        background: #f8ed28;
      }

      .low {
        color: #000000;
        background: #dbf947;
      }

      .lower {
        color: #000000;
        background: #bdff01;
      }
    }
  }
</style>
