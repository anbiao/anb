<template>
  <div class="redirect-button-wrapper">
    <dv-decoration-11 style="width:200px;height:60px;">
      <a-button type="link" href="/sensitive">
        综合态势感知
      </a-button>
    </dv-decoration-11>
    <dv-decoration-11 style="width:200px;height:60px;">
      <a-button type="link" href="/sensitive/data/">
        数据态势感知
      </a-button>
    </dv-decoration-11>
  </div>
</template>

<script>
export default {
  name: 'redirectButton'
}
</script>

<style lang="less">
.redirect-button-wrapper {
  position: absolute;
  display: flex;
  justify-content: space-between;
  align-items: center;
  height: 60px;
  top: 2.75rem;
  width: 100%;
  padding: 0 18%;

  .ant-btn-link {
    color: white;
    font-weight: 600;
  }
}
</style>