<template>
  <div class="center-cmp">

    <div class="cc-details">
      <div>全国大宗商品交易总数</div>
      <div class="card">2</div>
      <div class="card">1</div>
      <div class="card">3</div>
      <div class="card">7</div>
    </div>
    <div class="cc-details">
      <div>在线交易商数量</div>
      <div class="card">0</div>
      <div class="card">4</div>
      <div class="card">1</div>
      <div class="card">2</div>
    </div>
    <div class="cc-main-container">
      <div class="cc-sub-container">

        <dv-active-ring-chart class="ccmc-middle" :config="config_1" />

        <div class="ccmc-header">不同领域交易量占比</div>
        <LabelTag :config="labelConfig" />
      </div>
      <div class="cc-sub-container">

        <dv-active-ring-chart class="ccmc-middle" :config="config_2" />

        <div class="ccmc-header">不同领域交易交易商活跃度占比</div>
        <LabelTag :config="labelConfig" />
      </div>
    </div>
  </div>
</template>

<script>
import LabelTag from './LabelTag'

export default {
  name: 'CenterCmp',
  components: {
    LabelTag
  },
  data () {
    return {
      config_1: {
        data: [
          {
            name: '农林牧渔',
            value: 1315
          },
          {
            name: '金属矿产',
            value: 415
          },
          {
            name: '稀有金属',
            value: 90
          },
          {
            name: '能源化工',
            value: 317
          }
        ],
        color: ['#00baff', '#3de7c9', '#fff', '#ffc530'],
        lineWidth: 30,
        radius: '55%',
        activeRadius: '60%'
      },

      config_2: {
        data: [
          {
            name: '农林牧渔',
            value: 125
          },
          {
            name: '金属矿产',
            value: 115
          },
          {
            name: '稀有金属',
            value: 96
          },
          {
            name: '能源化工',
            value: 80
          }
        ],
        color: ['#00baff', '#3de7c9', '#fff', '#ffc530'],
        lineWidth: 30,
        radius: '55%',
        activeRadius: '60%'
      },

      labelConfig: {
        data: ['农林牧渔', '金属矿产', '稀有金属', '能源化工']
      }
    }
  }
}
</script>

<style lang="less">
.center-cmp {
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;


  .cc-details {
    height: 120px;
    display: flex;
    justify-content: center;
    font-size: 32px;
    align-items: center;

    .card {
      background-color: rgba(4,49,128,.6);
      color: #08e5ff;
      height: 70px;
      width: 70px;
      font-size: 45px;
      font-weight: bold;
      line-height: 70px;
      text-align: center;
      margin: 10px;
    }
  }
  .cc-main-container{
    width: 100%;
    height: 90%;
    display: flex;
    flex-direction: row;
    .cc-sub-container {
      width: 100%;
      height: 100%;
      flex-direction: column;
      transform: translateX(-10%);

      .ccmc-middle {
        width: 100%;
        height: 80%;
        flex-direction: column;
        .active-ring-name {
          font-size: 20px !important;
        }/*环状图每部分名称显示*/
      }/*环状图*/

      .ccmc-header{
        font-size: 20px;
        text-align: center;
      }/*环状图名称*/
      .label-tag {
        width: 500px;
        height: 30px;
        bottom: 20px;
      }

    }
  }
}
</style>
