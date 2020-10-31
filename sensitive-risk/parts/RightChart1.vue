<template>
  <div class="right-chart-1">
    <div class="rc1-header">农林牧渔产业交易异常分布</div>

    <div class="rc1-chart-container">
      <div class="left">
        <div>总交易异常件数</div>
        <div class="number">113</div>
      </div>

      <dv-charts class="right" :option="option" />
    </div>
  </div>
</template>

<script>
  export default {
    name: 'RightChart1',
    data () {
      return {
        option: {
          series: [
            {
              type: 'pie',
              data: [
                { name: '东北地区', value: 24 },
                { name: '华中地区', value: 36 },
                { name: '华南地区', value: 17 },
                { name: '西北地区', value: 23 },
                { name: '其他', value: 13 }
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
  .right-chart-1 {
    width: 100%;
    height: 100%;
    display: flex;
    flex-direction: column;

    .rc1-header {
      font-size: 24px;
      font-weight: bold;
      height: 30px;
      line-height: 30px;
    }

    .rc1-chart-container {
      flex: 1;
      display: flex;
    }

    .left {
      width: 30%;
      font-size: 18px;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;

      .number {
        font-size: 35px;
        color: #096dd9;
        font-weight: bold;
        margin-bottom: 20px;
      }
    }

    .right {
      flex: 1;
      padding-bottom: 20px;
      padding-right: 20px;
      box-sizing: border-box;
    }
  }
</style>
