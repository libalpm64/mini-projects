return {
  "folke/snacks.nvim",
  opts = {
    terminal = {
      win = {
        position = "bottom",
        height = 0.3,
        border = "rounded",
        wo = {
          winblend = 15,
          winhighlight = "Normal:Normal,FloatBorder:FloatBorder",
        },
      },
    },
  },
  keys = {
    { "<leader>t", function() require("snacks").terminal.toggle() end, desc = "Toggle terminal" },
    { "<leader>T", function() require("snacks").terminal.toggle({ cwd = vim.fn.getcwd() }) end, desc = "Toggle terminal (root)" },
    { "<Esc>", "<C-\\><C-n>", mode = "t", desc = "Exit terminal mode" },
  },
  config = function(_, opts)
    local snacks = require("snacks")
    snacks.setup(opts)
    vim.api.nvim_create_user_command("T", function()
      snacks.terminal.toggle()
    end, { desc = "Toggle terminal" })
    vim.cmd.cnoreabbrev("t", "T")
  end,
}
