<template>
  <div class="left-chart-1">
    <div class="lc1-header">交易异常量行业分布</div>
    <div class="lc1-details">总交易异常件数<span>512</span></div>
    <dv-charts class="lc1-chart" :option="option" />
    <dv-decoration-2 style="height:10px;" />
  </div>
</template>

<script>
export default {
  name: 'LeftChart1',
  data () {
    return {
      option: {
        series: [
          {
            type: 'pie',
            data: [
              { name: '农林牧渔', value: 113 },
              { name: '金属矿产', value: 90 },
              { name: '稀有金属', value: 137 },
              { name: '能源化工', value: 125 },
              { name: '其他', value: 47 }
            ],
            radius: ['45%', '65%'],
            insideLabel: {
              show: false
            },
            outsideLabel: {
              labelLineEndLength: 10,
              formatter: '{percent}%\n{name}',
              style: {
                fontSize: 16,
                fill: '#fff'
              }
            }
          }
        ],
        color: ['#00baff', '#3de7c9', '#fff', '#ffc530', '#469f4b']
      }
    }
  }
}
</script>

<style lang="less">
.left-chart-1 {
  width: 100%;
  height: 30%;
  display: flex;
  flex-direction: column;

  .lc1-header {
    height: 20px;
    line-height: 20px;
    font-size: 18px;
    text-indent: 20px;
    margin-top: 10px;
  }

  .lc1-details {
    height: 40px;
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

  .lc1-chart {
    height: calc(~"100% - 80px");
  }
}
</style>
