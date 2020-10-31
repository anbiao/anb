<template>
  <div class="left-chart-2">
    <div class="lc2-header">农林牧渔产业各风险类型指标</div>
    <div class="lc2-details">交易异常件数<span>113</span></div>
    <dv-capsule-chart class="lc2-chart" :config="config" />
    <dv-decoration-2 style="height:10px;" />
  </div>
</template>

<script>
export default {
  name: 'LeftChart2',
  data () {
    return {
      config: {
        data: [
          {
            name: '价格波动风险',
            value: 13
          },
          {
            name: '交易履约风险',
            value: 35
          },
          {
            name: '套期保值风险',
            value: 25
          },
          {
            name: '投机性风险',
            value: 27
          },
          {
            name: '流动性风险',
            value: 10
          }
        ],
        colors: ['#00baff', '#3de7c9', '#fff', '#ffc530', '#469f4b'],
        unit: ''
      }
    }
  }
}
</script>

<style lang="less">
.left-chart-2 {
  width: 100%;
  height: 37%;
  display: flex;
  flex-grow: 0;
  flex-direction: column;

  .lc2-header {
    text-align: center;
    height: 40px;
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 25px;
    margin-bottom: 20px;
  }

  .lc2-details {
    height: 50px;
    font-size: 18px;
    display: flex;
    align-items: center;
    text-indent: 20px;

    span {
      color: #096dd9;
      font-weight: bold;
      font-size: 30px;
      margin-left: 10px;
    }
  }

  .lc2-chart {
    flex: 1;
  }
}
</style>
