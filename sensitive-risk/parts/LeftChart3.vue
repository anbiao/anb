<template>
  <div class="left-chart-3">
    <div class="lc3-header">稀有金属产业各风险类型指标</div>
    <div class="lc3-details">交易异常件数<span>137</span></div>
    <dv-capsule-chart class="lc3-chart" :config="config" />
    <dv-decoration-2 style="height:10px;" />
  </div>
</template>

<script>
  export default {
    name: 'LeftChart3',
    data () {
      return {
        config: {
          data: [
            {
              name: '价格波动风险',
              value: 25
            },
            {
              name: '交易履约风险',
              value: 35
            },
            {
              name: '套期保值风险',
              value: 16
            },
            {
              name: '投机性风险',
              value: 34
            },
            {
              name: '流动性风险',
              value: 37
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
  .left-chart-3 {
    width: 100%;
    height: 37%;
    display: flex;
    flex-grow: 0;
    flex-direction: column;

    .lc3-header {
      text-align: center;
      height: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      font-size: 25px;
      margin-bottom: 20px;
    }

    .lc3-details {
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

    .lc3-chart {
      flex: 1;
    }
  }
</style>
