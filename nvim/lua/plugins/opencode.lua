return {
  "nickjvandyke/opencode.nvim",
  version = "*",
  dependencies = {
    {
      "folke/snacks.nvim",
      optional = true,
      opts = {
        terminal = {},
      },
    },
  },
  config = function()
    vim.o.autoread = true
    local default_width = 0.4
    local current_width = default_width

    local term_bufnr = nil
    local term_winid = nil

    local function start_server()
      local width = math.floor(vim.o.columns * current_width)
      local prev_win = vim.api.nvim_get_current_win()

      term_bufnr = vim.api.nvim_create_buf(true, false)

      -- botright vsplit forces the window to the far right, unlike
      -- nvim_open_win's split="right" which depends on current window layout
      vim.cmd("botright vsplit")
      vim.api.nvim_win_set_buf(0, term_bufnr)
      vim.api.nvim_win_set_width(0, width)
      term_winid = vim.api.nvim_get_current_win()

      require("opencode.terminal").setup(term_winid)

      vim.fn.jobstart("opencode --port", {
        term = true,
        on_exit = function()
          term_winid = nil
          term_bufnr = nil
        end,
      })

      vim.api.nvim_set_current_win(prev_win)
    end

    local function stop_server()
      local job_id = term_bufnr and vim.b[term_bufnr].terminal_job_id
      if job_id then vim.fn.jobstop(job_id) end
      if term_winid and vim.api.nvim_win_is_valid(term_winid) then
        vim.api.nvim_win_close(term_winid, true)
      end
      if term_bufnr and vim.api.nvim_buf_is_valid(term_bufnr) then
        vim.api.nvim_buf_delete(term_bufnr, { force = true })
      end
      term_winid = nil
      term_bufnr = nil
    end

    local function toggle_server()
      if term_bufnr == nil then
        start_server()
      else
        if term_winid ~= nil and vim.api.nvim_win_is_valid(term_winid) then
          vim.api.nvim_win_hide(term_winid)
          term_winid = nil
        elseif term_bufnr ~= nil and vim.api.nvim_buf_is_valid(term_bufnr) then
          local prev_win = vim.api.nvim_get_current_win()
          vim.cmd("botright vsplit")
          vim.api.nvim_win_set_buf(0, term_bufnr)
          vim.api.nvim_win_set_width(0, math.floor(vim.o.columns * current_width))
          term_winid = vim.api.nvim_get_current_win()
          vim.api.nvim_set_current_win(prev_win)
        end
      end
    end

    vim.g.opencode_opts = {
      server = {
        port = 4096,
        start = start_server,
        stop = stop_server,
        toggle = toggle_server,
      },
    }

    local function resize_opencode(size)
      if not size then return end
      local width = size / 100
      current_width = width
      if term_winid and vim.api.nvim_win_is_valid(term_winid) then
        vim.api.nvim_win_set_width(term_winid, math.floor(vim.o.columns * width))
      end
    end

    local function smart_toggle()
      if term_bufnr ~= nil and vim.api.nvim_buf_is_valid(term_bufnr) then
        require("opencode").stop()
        vim.system({ "pkill", "-f", "opencode" }, { detach = true })
        vim.system({ "pkill", "-f", "gopls.*opencode" }, { detach = true })
        vim.system({ "pkill", "-f", "jdtls.*opencode" }, { detach = true })
      else
        require("opencode").toggle()
      end
    end

    local function opencode_command(opts)
      local args = opts.args
      if args and args ~= "" then
        local size = tonumber(args)
        if size then
          resize_opencode(size)
        end
      else
        smart_toggle()
      end
    end

    vim.api.nvim_create_user_command("Opencode", opencode_command, {
      nargs = "?",
      desc = "Toggle opencode",
      complete = function()
        return { "30", "40", "50", "60", "70" }
      end,
    })

    vim.cmd.cnoreabbrev("opencode", "Opencode")

    vim.api.nvim_create_user_command("OpencodeSize", function(opts)
      local size = tonumber(opts.args)
      if size then
        resize_opencode(size)
      end
    end, {
      nargs = "?",
      desc = "Resize opencode window",
      complete = function()
        return { "30", "40", "50", "60", "70" }
      end,
    })

    vim.keymap.set("n", "<leader>o", smart_toggle, { desc = "Toggle opencode" })
  end,
}
